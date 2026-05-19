/*******************************************************
 * ESP32 智能家居控制系统（最终稳定版主程序）
 * 开发环境：Arduino IDE
 * 框架：Arduino
 *
 * 本文件只保留 setup()、loop() 和顶层调度。
 * 业务模块拆分到对应 .h / .cpp 文件中，便于毕业设计维护和答辩说明。
 *******************************************************/

#include <Arduino.h>
#include "config.h"
#include "globals.h"
#include "outputs.h"
#include "sensors.h"
#include "logic.h"
#include "display.h"
#include "mqtt_handler.h"
#include "voice_handler.h"
#include "utils.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("========================================");
  Serial.println("ESP32 智能家居控制系统 - 稳定版");
  Serial.println("========================================");

  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset Reason Code = ");
  Serial.println((int)reason);
  Serial.print("Reset Reason Text = ");
  Serial.println(resetReasonToString(reason));

  setupPins();
  setupSensors();

#if ENABLE_OLED
  setupOLED();
#endif

#if ENABLE_SERVO
  setupServo();
#endif

  setupWiFi();

#if ENABLE_MQTT
  setupMQTT();
#endif

#if ENABLE_VOICE
  setupVoiceSerial();
#endif

  setRedLed(false);
  setGreenLed(true);
  setBuzzer(false);
#if ENABLE_FAN
  // 上电默认关闭风扇，避免误动作
  setFan(false);
#endif

  Serial.println("[OK] 系统初始化完成");
}

void loop() {
  unsigned long now = millis();

  reconnectWiFiIfNeeded();

#if ENABLE_MQTT
  reconnectMQTTIfNeeded();
  mqttClient.loop();
#endif

#if ENABLE_VOICE
  handleVoiceCommand();
#endif

#if ENABLE_DHT11
  if (now - tDht >= DHT_INTERVAL_MS) {
    tDht = now;
    readDHTTask();
  }
#endif

#if ENABLE_BH1750
  if (now - tLux >= BH1750_INTERVAL_MS) {
    tLux = now;
    readBH1750Task();
  }
#endif

#if ENABLE_MQ2
  if (now - tMq2 >= MQ2_INTERVAL_MS) {
    tMq2 = now;
    readMQ2Task();
  }
#endif

#if ENABLE_DOOR
  if (now - tDoor >= DOOR_INTERVAL_MS) {
    tDoor = now;
    readDoorTask();
  }
#endif

#if ENABLE_PIR
  if (now - tPir >= PIR_INTERVAL_MS) {
    tPir = now;
    readPirTask();
  }
#endif

#if ENABLE_RAIN
  if (now - tRain >= RAIN_INTERVAL_MS) {
    tRain = now;
    readRainTask();
  }
#endif

  updateLogic();
  applyOutputs();

#if ENABLE_OLED
  if (now - tOled >= OLED_INTERVAL_MS) {
    tOled = now;
    updateOLEDTask();
  }
#endif

#if ENABLE_MQTT
  if (now - tPub >= MQTT_PUB_INTERVAL) {
    tPub = now;
    publishStatusTask();
  }
#endif

  if (now - tSerial >= SERIAL_PRINT_MS) {
    tSerial = now;
    printSystemStatus();
  }
}
