#pragma once

#include <Arduino.h>
#include "types.h"
#include "esp_system.h"

void clearAlarmState();
void printSystemStatus();
String modeToString(SystemMode mode);
String resetReasonToString(esp_reset_reason_t reason);
