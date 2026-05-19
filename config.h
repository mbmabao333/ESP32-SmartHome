#pragma once

#include <Arduino.h>
#include <DHT.h>

// ========================= 功能开关 =========================
#define ENABLE_DHT11   1
#define ENABLE_BH1750  1
#define ENABLE_MQ2     1
#define ENABLE_DOOR    1
#define ENABLE_PIR     1
#define ENABLE_RAIN    1
#define ENABLE_OLED    1
#define ENABLE_SERVO   1
#define ENABLE_FAN     1
#define ENABLE_MQTT    1
#define ENABLE_VOICE   1

// ========================= 传感器配置 =========================
#define DHT_TYPE         DHT11

// ========================= 网络与 MQTT 配置 =========================
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASS        "YOUR_WIFI_PASSWORD"

#define MQTT_HOST        "YOUR_MQTT_HOST"
#define MQTT_PORT        8883
#define MQTT_USER        "YOUR_MQTT_USER"
#define MQTT_PASS        "YOUR_MQTT_PASSWORD"
#define MQTT_CLIENT_ID   "esp32_smarthome_01"

// MQTT 上报主题：保持当前稳定版主题不变
#define TOPIC_STATE         "mb/esp32/smarthome/state"
#define TOPIC_TEMP          "mb/esp32/smarthome/sensor/temp"
#define TOPIC_HUMI          "mb/esp32/smarthome/sensor/humi"
#define TOPIC_LUX           "mb/esp32/smarthome/sensor/lux"
#define TOPIC_MQ2           "mb/esp32/smarthome/sensor/mq2"
#define TOPIC_DOOR          "mb/esp32/smarthome/security/door"
#define TOPIC_PIR           "mb/esp32/smarthome/security/pir"
#define TOPIC_RAIN          "mb/esp32/smarthome/env/rain"
#define TOPIC_ALARM         "mb/esp32/smarthome/security/alarm"
#define TOPIC_SMOKE_ALARM   "mb/esp32/smarthome/security/smoke_alarm"
#define TOPIC_STATUS_JSON   "mb/esp32/smarthome/status/json"

// MQTT 命令主题：保持当前稳定版主题不变
#define TOPIC_CMD_MODE      "mb/esp32/smarthome/cmd/mode"
#define TOPIC_CMD_BUZZER    "mb/esp32/smarthome/cmd/buzzer"
#define TOPIC_CMD_LED       "mb/esp32/smarthome/cmd/led"
#define TOPIC_CMD_SERVO     "mb/esp32/smarthome/cmd/servo"
#define TOPIC_CMD_FAN       "mb/esp32/smarthome/cmd/fan"
#define TOPIC_CMD_ALARM_CLR "mb/esp32/smarthome/cmd/alarm_clear"

// ========================= 周期参数 =========================
constexpr unsigned long DHT_INTERVAL_MS     = 2000;
constexpr unsigned long BH1750_INTERVAL_MS  = 1000;
constexpr unsigned long MQ2_INTERVAL_MS     = 500;
constexpr unsigned long DOOR_INTERVAL_MS    = 100;
constexpr unsigned long PIR_INTERVAL_MS     = 100;
constexpr unsigned long RAIN_INTERVAL_MS    = 200;
constexpr unsigned long OLED_INTERVAL_MS    = 1000;
constexpr unsigned long MQTT_PUB_INTERVAL   = 5000;
constexpr unsigned long WIFI_RETRY_INTERVAL = 5000;
constexpr unsigned long MQTT_RETRY_INTERVAL = 5000;
constexpr unsigned long SERIAL_PRINT_MS     = 1000;
constexpr unsigned long VOICE_BAUD_RATE     = 115200;

// ========================= 业务阈值与动作参数 =========================
constexpr int MQ2_INVALID_HIGH   = 4090;
constexpr int MQ2_WARN_THRESHOLD = 3200;
constexpr int MQ2_ALARM_THRESHOLD = 3600;
constexpr unsigned long SMOKE_ALARM_HOLD_MS = 3000;
constexpr int MQ2_SAMPLE_COUNT   = 10;

// 自动模式：光照低于该阈值时点亮绿灯
constexpr float AUTO_LUX_LOW_THRESHOLD = 100.0f;

constexpr int SERVO_NORMAL_ANGLE = 0;
constexpr int SERVO_ALARM_ANGLE  = 90;
constexpr int SERVO_RAIN_ANGLE   = 90;

constexpr int OLED_PAGE_COUNT    = 3;
