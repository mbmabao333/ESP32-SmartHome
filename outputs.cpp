#include <Arduino.h>
#include <ESP32Servo.h>

#include "outputs.h"
#include "globals.h"

void setupPins() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
#if ENABLE_FAN
  pinMode(FAN_PIN, OUTPUT);
#endif

#if ENABLE_DOOR
  pinMode(DOOR_PIN, INPUT);
#endif

#if ENABLE_PIR
  // HC-SR501 输出为数字量，高电平表示检测到人体
  pinMode(PIR_PIN, INPUT);
#endif

#if ENABLE_RAIN
  // 雨滴继电器输出使用上拉输入，LOW 表示检测到下雨
  pinMode(RAIN_PIN, INPUT_PULLUP);
#endif

#if ENABLE_MQ2
  pinMode(MQ2_PIN, INPUT);
#endif

  Serial.println("[OK] GPIO 初始化完成");
}

void setupServo() {
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
  setServoAngleSafe(SERVO_NORMAL_ANGLE);
  Serial.println("[OK] 舵机初始化完成");
}

void applyOutputs() {
  // 最终输出优先级：主报警 > 雨水关窗 > 模式内联动/手动 > 默认状态。
  // 雨水、PIR、SmokeWarn 本身不直接进入 Alarm，Alarm 只由 DoorAlarm 或 SmokeAlarm 产生。
  if (g_alarm) {
    applyAlarmOutput(true);
  } else {
    // 红灯、蜂鸣器：无主报警时只允许 Manual 手动控制，其余模式默认关闭。
    setRedLed(g_mode == MODE_MANUAL ? g_manualRedLed : false);
    setBuzzer(g_mode == MODE_MANUAL ? g_manualBuzzer : false);

    // 绿灯：主报警关闭优先；无报警时 Auto 光照联动优先，其次 Manual 手动。
    if (g_mode == MODE_AUTO) {
      setGreenLed(g_lux < AUTO_LUX_LOW_THRESHOLD);
    } else if (g_mode == MODE_MANUAL) {
      setGreenLed(g_manualGreenLed);
    } else {
      setGreenLed(true);
    }

#if ENABLE_SERVO
    // 舵机：主报警 > 雨水关窗 > Manual 手动 > 默认角度。
    if (g_rainTriggered) {
      setServoAngleSafe(SERVO_RAIN_ANGLE);
    } else if (g_mode == MODE_MANUAL) {
      setServoAngleSafe(g_manualServoAngle);
    } else {
      setServoAngleSafe(SERVO_NORMAL_ANGLE);
    }
#endif
  }

#if ENABLE_FAN
  // 风扇不放入 Security 主报警执行链：Auto 跟随 SmokeWarn，Manual 跟随手动命令。
  if (g_mode == MODE_AUTO) {
    setFan(g_smokeWarn);
  } else if (g_mode == MODE_MANUAL) {
    setFan(g_manualFan);
  } else {
    setFan(false);
  }
#endif
}

void applyAlarmOutput(bool alarmOn) {
  // 主报警执行链只控制红灯、绿灯、蜂鸣器和舵机；风扇由模式优先级单独处理。
  if (alarmOn) {
    setRedLed(true);
    setGreenLed(false);
    setBuzzer(true);
#if ENABLE_SERVO
    setServoAngleSafe(SERVO_ALARM_ANGLE);
#endif
  } else {
    setRedLed(false);
    setGreenLed(true);
    setBuzzer(false);
#if ENABLE_SERVO
    setServoAngleSafe(SERVO_NORMAL_ANGLE);
#endif
  }
}

void setBuzzer(bool on) {
  // 低电平触发：LOW=响，HIGH=不响
  digitalWrite(BUZZER_PIN, on ? LOW : HIGH);
}

void setRedLed(bool on) {
  digitalWrite(RED_LED_PIN, on ? HIGH : LOW);
}

void setGreenLed(bool on) {
  digitalWrite(GREEN_LED_PIN, on ? HIGH : LOW);
}

void setServoAngleSafe(int angle) {
#if ENABLE_SERVO
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  myServo.write(angle);
#endif
}

void setFan(bool on) {
#if ENABLE_FAN
  // MOS 驱动板测试结论：GPIO16 = HIGH 开启，LOW 关闭
  digitalWrite(FAN_PIN, on ? HIGH : LOW);
#endif
}
