# 基于 ESP32 的智能家居控制系统

本科毕业设计 | 安徽建筑大学 · 物联网工程

## 项目简介

基于 ESP32 开发板，实现集环境监测、安防联动、MQTT 云端远程控制与语音交互于一体的智能家居控制系统。系统采用模块化 C++ 架构，使用 `millis()` 非阻塞调度框架并发管理多路任务，通过 EMQX 云平台实现远程监控与控制。

## 功能模块

| 模块 | 功能描述 |
|---|---|
| 环境监测 | DHT11 采集温湿度，BH1750 采集光照强度，OLED 实时显示 |
| 安防检测 | 门磁传感器检测开关状态，MQ-2 双级阈值烟雾/气体预警 |
| 设备联动 | 蜂鸣器、红/绿 LED、舵机、风扇根据模式自动联动 |
| 三种工作模式 | 手动模式 / 自动模式（光控照明）/ 安防模式（门磁报警） |
| MQTT 云控 | TLS 加密接入 EMQX，每 5s 以 JSON 格式上报完整状态，支持远程指令控制 |
| 语音交互 | UART 接入 ASR01 模块，支持 14 条语音指令（灯控、风扇、舵机、模式切换、消警等） |

## 硬件清单

- ESP32 30PIN 开发板
- DHT11 温湿度传感器
- BH1750 / GY-30 光照传感器
- MQ-2 烟雾/可燃气体传感器
- 门磁模块
- OLED 显示屏（128×64，I2C）
- ASR01 离线语音识别模块
- 有源蜂鸣器（低电平触发）
- 红色 / 绿色 LED
- SG90 舵机
- 风扇 + MOS 驱动模块

## 引脚接线

| 模块 | ESP32 引脚 |
|---|---|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| DHT11 | GPIO 4 |
| BH1750 SDA/SCL | GPIO 21 / 22（共用 I2C）|
| MQ-2 | GPIO 34（ADC）|
| 门磁 | GPIO 27 |
| 蜂鸣器 | GPIO 26 |
| 红色 LED | GPIO 18 |
| 绿色 LED | GPIO 19 |
| 舵机 | GPIO 23 |
| 风扇 | GPIO 16 |
| ASR01 RX | GPIO 17 |

## 系统架构

```
传感器采集 → 状态判断 → OLED 显示
                ↓
           本地联动执行（蜂鸣器 / LED / 舵机 / 风扇）
                ↓
           MQTT 状态上报（JSON）→ 云端 / 手机 App
                ↑
           远程指令下发（模式切换 / 设备控制）
           语音指令输入
```

## 代码结构

```
SmartHome/
├── SmartHome.ino       # 主程序（setup / loop 调度）
├── config.h            # WiFi / MQTT 配置、功能开关、业务阈值
├── pins.h              # 硬件引脚定义
├── globals.h/cpp       # 全局状态变量
├── sensors.h/cpp       # 传感器采集任务
├── logic.h/cpp         # 业务逻辑（模式判断、报警逻辑）
├── outputs.h/cpp       # 执行器输出控制
├── display.h/cpp       # OLED 显示
├── mqtt_handler.h/cpp  # WiFi / MQTT 连接与收发
├── voice_handler.h/cpp # ASR01 语音指令解析
└── utils.h/cpp         # 工具函数
```

## 快速开始

1. 克隆仓库
   ```bash
   git clone https://github.com/mbmabao333/ESP32-SmartHome.git
   ```

2. 用 Arduino IDE 打开 `SmartHome.ino`

3. 修改 `config.h` 中的配置项：
   ```cpp
   #define WIFI_SSID   "你的WiFi名称"
   #define WIFI_PASS   "你的WiFi密码"
   #define MQTT_HOST   "你的MQTT服务器地址"
   #define MQTT_USER   "你的MQTT用户名"
   #define MQTT_PASS   "你的MQTT密码"
   ```

4. 按需开启/关闭功能（`config.h` 顶部的 `ENABLE_*` 宏）

5. 编译上传至 ESP32

## 开发环境

- Arduino IDE 2.x
- ESP32 Arduino Core
- 主要依赖库：`DHT sensor library`、`BH1750`、`Adafruit SSD1306`、`PubSubClient`、`ESP32Servo`
