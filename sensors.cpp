#include <Arduino.h>
#include <Wire.h>

#include "sensors.h"
#include "globals.h"
#include "mqtt_handler.h"

void setupSensors() {
#if ENABLE_DHT11
  dht.begin();
  Serial.println("[OK] DHT11 初始化完成");
#endif

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.println("[OK] I2C 初始化完成");

#if ENABLE_BH1750
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("[OK] BH1750 初始化成功");
  } else {
    Serial.println("[WARN] BH1750 初始化失败，请检查接线");
  }
#endif
}

void readDHTTask() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) {
    g_temp = t;
    g_lastValidTemp = t;
  } else {
    g_temp = g_lastValidTemp;
    Serial.println("[WARN] DHT11 温度读取失败，使用上次有效值");
  }

  if (!isnan(h)) {
    g_humi = h;
    g_lastValidHumi = h;
  } else {
    g_humi = g_lastValidHumi;
    Serial.println("[WARN] DHT11 湿度读取失败，使用上次有效值");
  }
}

void readBH1750Task() {
  float lux = lightMeter.readLightLevel();

  if (lux >= 0 && lux < 200000) {
    g_lux = lux;
    g_lastValidLux = lux;
  } else {
    g_lux = g_lastValidLux;
    Serial.println("[WARN] BH1750 读取异常，使用上次有效值");
  }
}

void readMQ2Task() {
  int raw = analogRead(MQ2_PIN);
  g_mq2Raw = raw;

  int validValue = raw;
  if (raw >= MQ2_INVALID_HIGH) {
    Serial.print("[WARN] MQ2 可疑高值: ");
    Serial.println(raw);
    validValue = g_lastValidMq2;
  } else {
    g_lastValidMq2 = raw;
  }

  g_mq2Buf[g_mq2BufIndex] = validValue;
  g_mq2BufIndex++;

  if (g_mq2BufIndex >= MQ2_SAMPLE_COUNT) {
    g_mq2BufIndex = 0;
    g_mq2BufFull = true;
  }

  int count = g_mq2BufFull ? MQ2_SAMPLE_COUNT : g_mq2BufIndex;
  long sum = 0;
  for (int i = 0; i < count; i++) {
    sum += g_mq2Buf[i];
  }

  g_mq2Avg = (count > 0) ? (sum / count) : validValue;
  g_smokeWarn = (g_mq2Avg >= MQ2_WARN_THRESHOLD);
}

void readDoorTask() {
  int val = digitalRead(DOOR_PIN);
  g_doorTriggered = (val == LOW);
}

void readPirTask() {
  static bool s_lastPirTriggered = false;

  // HC-SR501 单独作为状态检测模块，HIGH 表示检测到人体
  int val = digitalRead(PIR_PIN);
  g_pirTriggered = (val == HIGH);

  // PIR 当前为 L 模式，高电平持续时间有限。
  // 因此在状态变化时立即补发一次 MQTT，避免只靠周期上报错过触发窗口。
#if ENABLE_MQTT
  if (g_pirTriggered != s_lastPirTriggered && g_mqttOK) {
    publishStatusTask();
  }
#endif

  s_lastPirTriggered = g_pirTriggered;
}

void readRainTask() {
  // 已知接线：GPIO25 使用 INPUT_PULLUP，LOW=Rain Triggered，HIGH=No Rain
  int val = digitalRead(RAIN_PIN);
  g_rainTriggered = (val == LOW);
}
