
#include <ESP8266HTTPClient.h>
#include "time_manager.hpp"
#include "lcd.hpp"
#include <Arduino.h>
#include <time.h>

const int SECONDS_DAY = 24 * 60 * 60;

HTTPClient http;
const char* TZDB_API_URL = "http://api.timezonedb.com/v2/get-time-zone?key=SLHAXNXKNIPX&format=json&by=zone&zone=Europe/Berlin";

unsigned long sync_timestamp = 0;
unsigned long timestamp = 0;
unsigned long timestamp_base = 0;

int ts_parse_response(String json) {
    int tsPos = -1;

    if (json == NULL) {
        Serial.println("ERROR: invalid response");
        return -1;
    }

    tsPos = json.indexOf("\"timestamp\"");
    
    if (tsPos != -1) {
        int commaPos = json.indexOf(',', tsPos + 12);
        String tsStr = json.substring(tsPos + 12, commaPos);
        return tsStr.toInt();
    }

    Serial.println("ERROR: no timestamp found in response");

    return 0;
}

void tm_sync(uint32_t deltaTime) {
    String json = "\0";

    Serial.println("Requesting timestamp from timezonedb.com...");

    http.begin(TZDB_API_URL);

    int httpCode = http.GET();

    if (httpCode > 0) {
        json = http.getString();
        http.end();
        sync_timestamp = ts_parse_response(json);
        timestamp = sync_timestamp;
        timestamp_base = millis() / 1000;

        char time_str[32];
        tm_date_str(time_str);
        Serial.printf("Current time: %s\n", time_str);
        // lcd_print(0, 0, "timestamp:",1);
    }

    http.end();
    Serial.printf("ERROR: cannot get time, http response code is: %d", httpCode);
}

void tm_tick(uint32_t deltaTime) {
    if (sync_timestamp) {
        timestamp = sync_timestamp + millis() / 1000 - timestamp_base;
        tm_display();
    }
    Serial.printf("tick...\n");
}

void tm_date_str(char * str) {
    time_t ts = timestamp;
    struct tm * t = gmtime(&ts);
    sprintf(str, "%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
}

void tm_time_str(char * str) {
    time_t ts = timestamp;
    struct tm * t = gmtime(&ts);
    sprintf(str, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

void tm_display() {
    char date_str[32];
    char time_str[32];
    tm_date_str(date_str);
    tm_time_str(time_str);
    lcd_print(0, 0, date_str, 1);
    lcd_print(0, 1, time_str, 0);
}
