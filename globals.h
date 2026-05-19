#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#include "pins.h"
#include "config.h"
#include "types.h"

// ========================= 全局对象声明 =========================
extern DHT dht;
extern BH1750 lightMeter;
extern Adafruit_SSD1306 display;
extern Servo myServo;
extern WiFiClientSecure secureClient;
extern PubSubClient mqttClient;

// ========================= 全局状态声明 =========================
extern SystemMode g_mode;

extern float g_temp;
extern float g_humi;
extern float g_lux;
extern int g_mq2Raw;
extern int g_mq2Avg;

extern bool g_doorTriggered;
extern bool g_pirTriggered;
extern bool g_rainTriggered;
extern bool g_doorAlarm;
extern bool g_smokeWarn;
extern bool g_smokeAlarm;
extern bool g_alarm;

extern bool g_wifiOK;
extern bool g_mqttOK;

extern bool g_manualBuzzer;
extern bool g_manualRedLed;
extern bool g_manualGreenLed;
extern bool g_manualFan;
extern int g_manualServoAngle;

extern int g_oledPage;

extern float g_lastValidTemp;
extern float g_lastValidHumi;
extern float g_lastValidLux;
extern int g_lastValidMq2;

extern int g_mq2Buf[MQ2_SAMPLE_COUNT];
extern int g_mq2BufIndex;
extern bool g_mq2BufFull;

extern unsigned long tDht;
extern unsigned long tLux;
extern unsigned long tMq2;
extern unsigned long tDoor;
extern unsigned long tPir;
extern unsigned long tRain;
extern unsigned long tOled;
extern unsigned long tPub;
extern unsigned long tWiFi;
extern unsigned long tMqtt;
extern unsigned long tSerial;
extern unsigned long g_smokeAlarmStartMs;
