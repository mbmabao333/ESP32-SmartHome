#pragma once

#include <Arduino.h>

void setupWiFi();
void setupMQTT();
void reconnectWiFiIfNeeded();
void reconnectMQTTIfNeeded();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishStatusTask();
