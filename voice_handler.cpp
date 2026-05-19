#include <Arduino.h>

#include "voice_handler.h"
#include "globals.h"
#include "utils.h"

void setupVoiceSerial() {
  // ASR01 当前采用 UART 单向接收，ESP32 只接收模块 TX 输出的 1 字节命令码
  Serial2.begin(VOICE_BAUD_RATE, SERIAL_8N1, VOICE_RX_PIN, -1);
  Serial.print("[OK] ASR01 语音串口初始化完成, RX=GPIO");
  Serial.print(VOICE_RX_PIN);
  Serial.print(", Baud=");
  Serial.println(VOICE_BAUD_RATE);
}

void handleVoiceCommand() {
#if ENABLE_VOICE
  // 为避免破坏 Auto / Security 现有联动，设备控制命令统一复用手动变量。
  // 因此这些语音命令优先在 Manual 模式下生效，其他模式下只更新手动目标状态。
  while (Serial2.available() > 0) {
    uint8_t cmd = (uint8_t)Serial2.read();

    Serial.print("[VOICE] CMD=0x");
    if (cmd < 0x10) {
      Serial.print("0");
    }
    Serial.println(cmd, HEX);

    switch (cmd) {
      case 0xA1:
        g_mode = MODE_MANUAL;
        Serial.println("[VOICE] 切换到手动模式");
        break;
      case 0xA2:
        g_mode = MODE_AUTO;
        Serial.println("[VOICE] 切换到自动模式");
        break;
      case 0xA3:
        g_mode = MODE_SECURITY;
        Serial.println("[VOICE] 切换到安防模式");
        break;

      case 0xB1:
        g_manualGreenLed = true;
        Serial.println("[VOICE] 手动打开绿灯");
        break;
      case 0xB2:
        g_manualGreenLed = false;
        Serial.println("[VOICE] 手动关闭绿灯");
        break;
      case 0xB3:
        g_manualRedLed = true;
        Serial.println("[VOICE] 手动打开红灯");
        break;
      case 0xB4:
        g_manualRedLed = false;
        Serial.println("[VOICE] 手动关闭红灯");
        break;

      case 0xC1:
        g_manualFan = true;
        Serial.println("[VOICE] 手动打开风扇");
        break;
      case 0xC2:
        g_manualFan = false;
        Serial.println("[VOICE] 手动关闭风扇");
        break;

      case 0xD1:
        g_manualServoAngle = SERVO_NORMAL_ANGLE;
        Serial.println("[VOICE] 打开窗户");
        break;
      case 0xD2:
        g_manualServoAngle = SERVO_RAIN_ANGLE;
        Serial.println("[VOICE] 关闭窗户");
        break;

      case 0xE1:
        g_manualBuzzer = true;
        Serial.println("[VOICE] 手动打开蜂鸣器");
        break;
      case 0xE2:
        g_manualBuzzer = false;
        Serial.println("[VOICE] 手动关闭蜂鸣器");
        break;

      case 0xF1:
        clearAlarmState();
        Serial.println("[VOICE] 已执行消警");
        break;

      default:
        Serial.println("[VOICE] 未识别的命令码");
        break;
    }
  }
#endif
}
