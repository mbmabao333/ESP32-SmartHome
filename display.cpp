#include <Arduino.h>
#include <WiFi.h>

#include "display.h"
#include "globals.h"
#include "utils.h"

void setupOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("[WARN] OLED 初始化失败，请检查接线");
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Smart Home");
  display.println("Stable Version");
  display.display();

  Serial.println("[OK] OLED 初始化成功");
}

void updateOLEDTask() {
#if ENABLE_OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (g_oledPage == 0) {
    display.println("== Env Data ==");
    display.print("T: ");
    display.print(g_temp, 1);
    display.println(" C");

    display.print("H: ");
    display.print(g_humi, 1);
    display.println(" %");

    display.print("Lux: ");
    display.println(g_lux, 1);

    display.print("MQ2: ");
    display.println(g_mq2Avg);
  } else if (g_oledPage == 1) {
    display.println("== Security ==");
    display.print("Door: ");
    display.println(g_doorTriggered ? "TRIG" : "SAFE");

    display.print("PIR: ");
    display.println(g_pirTriggered ? "YES" : "NO");

    // 雨滴模块当前仅用于状态显示，不参与主报警联动
    display.print("Rain: ");
    display.println(g_rainTriggered ? "YES" : "NO");

    display.print("SmokeWarn: ");
    display.println(g_smokeWarn ? "YES" : "NO");

    display.print("SmokeAlarm: ");
    display.println(g_smokeAlarm ? "YES" : "NO");

    display.print("Alarm: ");
    display.println(g_alarm ? "ON" : "OFF");

    display.print("Mode: ");
    display.println(modeToString(g_mode));
  } else {
    display.println("== Network ==");
    display.print("WiFi: ");
    display.println(g_wifiOK ? "OK" : "OFF");

    display.print("MQTT: ");
    display.println(g_mqttOK ? "OK" : "OFF");

    display.print("IP: ");
    if (g_wifiOK) {
      display.println(WiFi.localIP());
    } else {
      display.println("N/A");
    }
  }

  display.display();

  g_oledPage++;
  if (g_oledPage >= OLED_PAGE_COUNT) {
    g_oledPage = 0;
  }
#endif
}
