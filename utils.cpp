#include <Arduino.h>
#include "esp_system.h"

#include "utils.h"
#include "globals.h"
#include "outputs.h"

void printSystemStatus() {
  Serial.print("Door=");
  Serial.print(g_doorTriggered ? 1 : 0);

  Serial.print(" DoorAlarm=");
  Serial.print(g_doorAlarm ? 1 : 0);

  Serial.print(" PIR=");
  Serial.print(g_pirTriggered ? 1 : 0);

  Serial.print(" Rain=");
  Serial.print(g_rainTriggered ? 1 : 0);

  Serial.print(" T=");
  Serial.print(g_temp, 1);

  Serial.print(" H=");
  Serial.print(g_humi, 1);

  Serial.print(" Lux=");
  Serial.print(g_lux, 1);

  Serial.print(" MQ2raw=");
  Serial.print(g_mq2Raw);

  Serial.print(" MQ2avg=");
  Serial.print(g_mq2Avg);

  Serial.print(" SmokeWarn=");
  Serial.print(g_smokeWarn ? 1 : 0);

  Serial.print(" SmokeAlarm=");
  Serial.print(g_smokeAlarm ? 1 : 0);

  Serial.print(" Alarm=");
  Serial.print(g_alarm ? 1 : 0);

  Serial.print(" Mode=");
  Serial.print(modeToString(g_mode));

  Serial.print(" WiFi=");
  Serial.print(g_wifiOK ? 1 : 0);

  Serial.print(" MQTT=");
  Serial.println(g_mqttOK ? 1 : 0);
}

void clearAlarmState() {
  // 统一复用当前系统消警处理，供 MQTT 与语音命令共同调用。
  // 这里只清除缓存和当前输出；若真实报警条件仍成立，下一轮 updateLogic() 会重新置位。
  g_alarm = false;
  g_doorAlarm = false;
  g_smokeAlarm = false;
  g_smokeAlarmStartMs = 0;
  g_manualBuzzer = false;
  g_manualRedLed = false;
  g_manualGreenLed = true;
  g_manualFan = false;
  g_manualServoAngle = SERVO_NORMAL_ANGLE;

  setBuzzer(false);
  setRedLed(false);
  setServoAngleSafe(SERVO_NORMAL_ANGLE);
}

String modeToString(SystemMode mode) {
  switch (mode) {
    case MODE_MANUAL:   return "MANUAL";
    case MODE_AUTO:     return "AUTO";
    case MODE_SECURITY: return "SECURITY";
    default:            return "UNKNOWN";
  }
}

String resetReasonToString(esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_UNKNOWN:   return "UNKNOWN";
    case ESP_RST_POWERON:   return "POWERON";
    case ESP_RST_EXT:       return "EXTERNAL";
    case ESP_RST_SW:        return "SOFTWARE";
    case ESP_RST_PANIC:     return "PANIC";
    case ESP_RST_INT_WDT:   return "INT_WDT";
    case ESP_RST_TASK_WDT:  return "TASK_WDT";
    case ESP_RST_WDT:       return "WDT";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP";
    case ESP_RST_BROWNOUT:  return "BROWNOUT";
    case ESP_RST_SDIO:      return "SDIO";
    default:                return "UNDEFINED";
  }
}
