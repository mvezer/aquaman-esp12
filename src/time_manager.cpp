
#include <ESP8266HTTPClient.h>
#include "time_manager.hpp"
#include "lcd.hpp"

HTTPClient http;
const char* TZDB_API_URL = "http://api.timezonedb.com/v2/get-time-zone?key=SLHAXNXKNIPX&format=json&by=zone&zone=Europe/Berlin";

int sync_timestamp = -1;
int sys_timestamp = -1;

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

int tm_sync() {
    String json = "\0";

    Serial.println("Requesting timestamp from timezonedb.com...");

    http.begin(TZDB_API_URL);

    int httpCode = http.GET();

    if (httpCode > 0) {
        json = http.getString();
        http.end();
        sync_timestamp = ts_parse_response(json);
        sys_timestamp = millis() / 1000;

        Serial.printf("Sync timestamp: %d, System timestamp: %d", sync_timestamp, sys_timestamp);
        // lcd_print(0, 0, "timestamp:",1);
        return 1;
    }

    http.end();
    Serial.printf("ERROR: cannot get time, http response code is: %d", httpCode);
    return 0;
}
