#include "lcd.hpp"
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int show;

int lcd_init() {
    int error;
    Wire.begin();
    Wire.beginTransmission(0x27);
    error = Wire.endTransmission();
    Serial.println("");
    Serial.println("");
    if (error == 0) {
        Serial.println("LCD found :)");

    } else {
        Serial.println("ERROR: LCD not found.");
    } // if

    lcd.begin(16, 2);
    lcd.setBacklight(255);

    return error;
} 

int lcd_print(int x, int y, String str, int clear) {
    if (clear) {
        lcd.clear();
    }

    lcd.setCursor(x, y);
    lcd.print(str);

    return 0;
}