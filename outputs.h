#pragma once

void setupPins();
void setupServo();
void applyOutputs();
void applyAlarmOutput(bool alarmOn);
void setBuzzer(bool on);
void setRedLed(bool on);
void setGreenLed(bool on);
void setServoAngleSafe(int angle);
void setFan(bool on);
