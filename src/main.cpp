#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "time_manager.hpp"
#include "scheduler.hpp"
#include "lcd.hpp"
#include "Task.h"
#include "ButtonTask.h"

const char* SSID     = "WILLY.TEL-3KLO71Q7";
const char* PASSWORD = "21211214932962960528";

TaskManager taskManager;

#define FEEDING_BUTTON_PIN 2
#define MAINTENANCE_BUTTON_PIN 14

void handle_feeding_button(ButtonState state);
void handle_maintenance_button(ButtonState state);

FunctionTask task_tm_tick(tm_tick, MsToTaskTime(1000));
FunctionTask task_tm_sync(tm_sync, MsToTaskTime(1000 * 60 * 30));
FunctionTask task_sch_update(sch_update, MsToTaskTime(1000));
ButtonTask task_feeding_button(handle_feeding_button, FEEDING_BUTTON_PIN);
ButtonTask task_maintenance_button(handle_maintenance_button, MAINTENANCE_BUTTON_PIN);

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
  sch_init();
  lcd_print(0, 0, " ", 1);
  sch_update(0);
  taskManager.StartTask(&task_tm_sync);
  taskManager.StartTask(&task_tm_tick);
  taskManager.StartTask(&task_sch_update);
  taskManager.StartTask(&task_feeding_button);
  taskManager.StartTask(&task_maintenance_button);
}

void loop() {
  taskManager.Loop();
}

void handle_feeding_button(ButtonState state) {
  if (state == ButtonState_Pressed) {
    sch_toggle_feeding();
  }
}

void handle_maintenance_button(ButtonState state) {
  if (state == ButtonState_Pressed) {
    sch_toggle_maintenance();
  }
}
