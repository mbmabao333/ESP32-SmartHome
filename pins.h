#pragma once

// ========================= 硬件引脚定义 =========================
// I2C
#define I2C_SDA_PIN      21
#define I2C_SCL_PIN      22

// DHT11
#define DHT_PIN          4

// 门磁
#define DOOR_PIN         27

// 继电器型雨滴模块（GPIO25，INPUT_PULLUP，LOW=触发下雨）
#define RAIN_PIN         25

// MQ-2 模拟量输入
#define MQ2_PIN          34

// HC-SR501 人体红外（HIGH=检测到人体，LOW=无人）
#define PIR_PIN          33

// 蜂鸣器（低电平触发）
#define BUZZER_PIN       26

// 指示灯
#define RED_LED_PIN      18
#define GREEN_LED_PIN    19

// 舵机
#define SERVO_PIN        23

// 风扇（MOS 驱动，高电平开启）
#define FAN_PIN          16

// ASR01 语音模块（UART 单向接收，GPIO17 接模块 TX）
#define VOICE_RX_PIN     17

// OLED
#define SCREEN_WIDTH     128
#define SCREEN_HEIGHT    64
#define OLED_ADDR        0x3C
