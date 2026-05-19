#include <Arduino.h>

#include "logic.h"
#include "globals.h"

void updateLogic() {
  // 双级烟雾逻辑：
  // 1. SmokeWarn 继续使用当前预警阈值
  // 2. SmokeAlarm 需要更高阈值且持续成立
  // 3. 最终 Alarm = DoorAlarm OR SmokeAlarm
  unsigned long now = millis();

  // 门磁仍然是主安防触发来源之一，只在安防模式下参与主报警
  g_doorAlarm = (g_mode == MODE_SECURITY) && g_doorTriggered;

  // 手动模式不自动触发烟雾主报警，但保留状态显示与上报
  if (g_mode != MODE_MANUAL) {
    if (g_mq2Avg >= MQ2_ALARM_THRESHOLD) {
      if (g_smokeAlarmStartMs == 0) {
        g_smokeAlarmStartMs = now;
      }

      g_smokeAlarm = ((now - g_smokeAlarmStartMs) >= SMOKE_ALARM_HOLD_MS);
    } else {
      g_smokeAlarm = false;
      g_smokeAlarmStartMs = 0;
    }
  } else {
    g_smokeAlarm = false;
    g_smokeAlarmStartMs = 0;
  }

  g_alarm = g_doorAlarm || g_smokeAlarm;
}
