#include <Arduino.h>
#include <WiFi.h>

#include "mqtt_handler.h"
#include "globals.h"
#include "utils.h"

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("[INFO] 正在连接 WiFi: ");
  Serial.println(WIFI_SSID);
}

void setupMQTT() {
  // 当前稳定版沿用不校验证书的连接方式，避免影响现有功能
  secureClient.setInsecure();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  Serial.println("[OK] MQTT TLS 客户端初始化完成");
  Serial.print("[INFO] MQTT Host: ");
  Serial.println(MQTT_HOST);
  Serial.print("[INFO] MQTT Port: ");
  Serial.println(MQTT_PORT);
}

void reconnectWiFiIfNeeded() {
  unsigned long now = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if (!g_wifiOK) {
      g_wifiOK = true;
      Serial.print("[OK] WiFi 已连接, IP = ");
      Serial.println(WiFi.localIP());
    }
    return;
  }

  g_wifiOK = false;

  if (now - tWiFi < WIFI_RETRY_INTERVAL) {
    return;
  }
  tWiFi = now;

  Serial.println("[WARN] WiFi 未连接，尝试重连...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void reconnectMQTTIfNeeded() {
  if (!g_wifiOK) {
    g_mqttOK = false;
    return;
  }

  if (mqttClient.connected()) {
    if (!g_mqttOK) {
      g_mqttOK = true;
      Serial.println("[OK] MQTT 已连接");
    }
    return;
  }

  g_mqttOK = false;

  unsigned long now = millis();
  if (now - tMqtt < MQTT_RETRY_INTERVAL) {
    return;
  }
  tMqtt = now;

  Serial.println("[WARN] MQTT 未连接，尝试 TLS 重连...");

  bool ok = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
  if (!ok) {
    Serial.print("[WARN] MQTT 连接失败, state=");
    Serial.println(mqttClient.state());
    return;
  }

  g_mqttOK = true;
  Serial.println("[OK] MQTT TLS 连接成功");

  mqttClient.subscribe(TOPIC_CMD_MODE);
  mqttClient.subscribe(TOPIC_CMD_BUZZER);
  mqttClient.subscribe(TOPIC_CMD_LED);
  mqttClient.subscribe(TOPIC_CMD_SERVO);
  mqttClient.subscribe(TOPIC_CMD_FAN);
  mqttClient.subscribe(TOPIC_CMD_ALARM_CLR);

  mqttClient.publish(TOPIC_STATE, "online", true);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  String topicStr = String(topic);

  Serial.print("[MQTT] Topic: ");
  Serial.print(topicStr);
  Serial.print(" | Payload: ");
  Serial.println(msg);

  if (topicStr == TOPIC_CMD_MODE) {
    msg.toLowerCase();

    if (msg == "manual") {
      g_mode = MODE_MANUAL;
      Serial.println("[CMD] 切换到手动模式");
    } else if (msg == "auto") {
      g_mode = MODE_AUTO;
      Serial.println("[CMD] 切换到自动模式");
    } else if (msg == "security") {
      g_mode = MODE_SECURITY;
      Serial.println("[CMD] 切换到安防模式");
    }
  } else if (topicStr == TOPIC_CMD_BUZZER) {
    msg.toLowerCase();

    if (msg == "on") {
      g_manualBuzzer = true;
      Serial.println("[CMD] 手动打开蜂鸣器");
    } else if (msg == "off") {
      g_manualBuzzer = false;
      Serial.println("[CMD] 手动关闭蜂鸣器");
    }
  } else if (topicStr == TOPIC_CMD_LED) {
    msg.toLowerCase();

    if (msg == "red_on") {
      g_manualRedLed = true;
    } else if (msg == "red_off") {
      g_manualRedLed = false;
    } else if (msg == "green_on") {
      g_manualGreenLed = true;
    } else if (msg == "green_off") {
      g_manualGreenLed = false;
    } else if (msg == "all_off") {
      g_manualRedLed = false;
      g_manualGreenLed = false;
    }

    Serial.println("[CMD] LED 手动命令已处理");
  } else if (topicStr == TOPIC_CMD_SERVO) {
    int angle = msg.toInt();
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    g_manualServoAngle = angle;

    Serial.print("[CMD] 设置舵机角度 = ");
    Serial.println(g_manualServoAngle);
  } else if (topicStr == TOPIC_CMD_FAN) {
    msg.toLowerCase();

    if (msg == "on") {
      g_manualFan = true;
      Serial.println("[CMD] 手动打开风扇");
    } else if (msg == "off") {
      g_manualFan = false;
      Serial.println("[CMD] 手动关闭风扇");
    }
  } else if (topicStr == TOPIC_CMD_ALARM_CLR) {
    msg.toLowerCase();
    if (msg == "clear" || msg == "1") {
      clearAlarmState();
      Serial.println("[CMD] 已收到消警命令");
    }
  }
}

void publishStatusTask() {
#if ENABLE_MQTT
  if (!g_mqttOK) {
    return;
  }

  mqttClient.publish(TOPIC_TEMP, String(g_temp, 1).c_str(), true);
  mqttClient.publish(TOPIC_HUMI, String(g_humi, 1).c_str(), true);
  mqttClient.publish(TOPIC_LUX, String(g_lux, 1).c_str(), true);
  mqttClient.publish(TOPIC_MQ2, String(g_mq2Avg).c_str(), true);
  mqttClient.publish(TOPIC_DOOR, g_doorTriggered ? "1" : "0", true);
  // PIR 当前只做状态检测与上报，不进入主报警逻辑
  mqttClient.publish(TOPIC_PIR, g_pirTriggered ? "1" : "0", true);
  // 雨滴模块当前只做状态上报，不进入主报警逻辑
  mqttClient.publish(TOPIC_RAIN, g_rainTriggered ? "1" : "0", true);
  mqttClient.publish(TOPIC_ALARM, g_alarm ? "1" : "0", true);
  mqttClient.publish(TOPIC_SMOKE_ALARM, g_smokeAlarm ? "1" : "0", true);

  String json = "{";
  json += "\"temp\":" + String(g_temp, 1) + ",";
  json += "\"humi\":" + String(g_humi, 1) + ",";
  json += "\"lux\":" + String(g_lux, 1) + ",";
  json += "\"mq2\":" + String(g_mq2Avg) + ",";
  json += "\"door\":" + String(g_doorTriggered ? 1 : 0) + ",";
  json += "\"pir\":" + String(g_pirTriggered ? 1 : 0) + ",";
  json += "\"doorAlarm\":" + String(g_doorAlarm ? 1 : 0) + ",";
  json += "\"rain\":" + String(g_rainTriggered ? 1 : 0) + ",";
  json += "\"smokeWarn\":" + String(g_smokeWarn ? 1 : 0) + ",";
  json += "\"smokeAlarm\":" + String(g_smokeAlarm ? 1 : 0) + ",";
  json += "\"alarm\":" + String(g_alarm ? 1 : 0) + ",";
  json += "\"wifi\":" + String(g_wifiOK ? 1 : 0) + ",";
  json += "\"mqtt\":" + String(g_mqttOK ? 1 : 0) + ",";
  json += "\"mode\":\"" + modeToString(g_mode) + "\"";
  json += "}";

  mqttClient.publish(TOPIC_STATUS_JSON, json.c_str(), true);
#endif
}
