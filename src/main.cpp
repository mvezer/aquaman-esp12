#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "time_manager.hpp"
#include "lcd.hpp"
#include "Task.h"

const char* SSID     = "WILLY.TEL-3KLO71Q7";
const char* PASSWORD = "21211214932962960528";

TaskManager taskManager;

FunctionTask task_tm_tick(tm_tick, MsToTaskTime(1000));
FunctionTask task_tm_sync(tm_sync, MsToTaskTime(1000 * 60 * 30));

void connectWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  int cursorpos = 0;
  lcd_print(0, 0, "Connecting...", 1);
  while (WiFi.status() != WL_CONNECTED) {
    lcd_print(cursorpos++, 1, ".", 0);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address:");
  lcd_print(0, 0, "IP address:", 1);
  Serial.println(String(WiFi.localIP().toString()));
  lcd_print(0, 1, String(WiFi.localIP().toString()), 0);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  lcd_init();
  connectWiFi();
  tm_sync(0);
  taskManager.StartTask(&task_tm_sync);
  taskManager.StartTask(&task_tm_tick);
  // pinMode(2, OUTPUT);     // Initialize GPIO2 pin as an output
}

void loop() {
  taskManager.Loop();
  // digitalWrite(2, LOW);   // Turn the LED on by making the voltage LOW
  // delay(250);            // Wait for a second
  // digitalWrite(2, HIGH);  // Turn the LED off by making the voltage HIGH
  // delay(500);            // Wait for two seconds
}