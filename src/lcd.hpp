#include <Arduino.h>
#ifndef LCD_HPP
#define LCD_HPP

int lcd_init();
int lcd_print(int x, int y, String str, int clear);

#endif
