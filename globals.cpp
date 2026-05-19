#include "globals.h"

// ========================= 全局对象定义 =========================
DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo myServo;

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

// ========================= 全局状态定义 =========================
SystemMode g_mode = MODE_SECURITY;

float g_temp = 0.0f;
float g_humi = 0.0f;
float g_lux = 0.0f;
int g_mq2Raw = 0;
int g_mq2Avg = 0;

bool g_doorTriggered = false;
bool g_pirTriggered = false;
bool g_rainTriggered = false;
bool g_doorAlarm = false;
bool g_smokeWarn = false;
bool g_smokeAlarm = false;
bool g_alarm = false;

bool g_wifiOK = false;
bool g_mqttOK = false;

bool g_manualBuzzer = false;
bool g_manualRedLed = false;
bool g_manualGreenLed = true;
bool g_manualFan = false;
int g_manualServoAngle = SERVO_NORMAL_ANGLE;

int g_oledPage = 0;

float g_lastValidTemp = 0.0f;
float g_lastValidHumi = 0.0f;
float g_lastValidLux = 0.0f;
int g_lastValidMq2 = 0;

int g_mq2Buf[MQ2_SAMPLE_COUNT] = {0};
int g_mq2BufIndex = 0;
bool g_mq2BufFull = false;

unsigned long tDht = 0;
unsigned long tLux = 0;
unsigned long tMq2 = 0;
unsigned long tDoor = 0;
unsigned long tPir = 0;
unsigned long tRain = 0;
unsigned long tOled = 0;
unsigned long tPub = 0;
unsigned long tWiFi = 0;
unsigned long tMqtt = 0;
unsigned long tSerial = 0;
unsigned long g_smokeAlarmStartMs = 0;
