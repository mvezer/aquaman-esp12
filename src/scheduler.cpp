#include "scheduler.hpp"
#include <Arduino.h>
#include "time_manager.hpp"
#include "lcd.hpp"

int light_state = -1;
int co2_state = -1;
int filter_state = -1;

int is_maintenance = 0;
unsigned long maintenance_ts = 0;
const unsigned long MAINTENANCE_TIMEOUT = 60 * 60 * 2;

int is_feeding = 0;
unsigned long feeding_ts = 0;
const unsigned long FEEDING_TIMEOUT = 60 * 15;

const int LIGHT_GPIO = 16;
const int CO2_GPIO = 12;
const int FILTER_GPIO = 0;

const unsigned long HOURS_SEC = 60*60;
const unsigned long MINUTES_SEC = 60;

const unsigned long LIGTH_LENGTH = 2
;
const unsigned long CO2_LENGTH = 2;
const unsigned long FILTER_LENGTH = 1;

void sch_update_pin(int gpio, int state);

schedule_item light[LIGTH_LENGTH] = {
    { { 7, 0, 0 }, 0, 1 },
    { { 22, 0, 0 }, 0, 0 }
};

schedule_item co2[2] = {
    { { 5, 30, 0 }, 0, 1 },
    { { 20, 30, 0 }, 0, 0 }
};

schedule_item filter[1] = {
    { { 0, 0, 0 }, 0, 1 },
};

unsigned long sch_create_timestamp(time_item t) {
    return t.hours * HOURS_SEC + t.minutes * MINUTES_SEC + t.seconds;
}

void sch_init() {
    int i = 0;

    pinMode(LIGHT_GPIO, OUTPUT);
    sch_update_pin(LIGHT_GPIO, 0);
    pinMode(CO2_GPIO, OUTPUT);
    sch_update_pin(CO2_GPIO, 0);
    pinMode(FILTER_GPIO, OUTPUT);
    sch_update_pin(FILTER_GPIO, 0);

    for (i = 0; i < LIGTH_LENGTH; i ++) {
        light[i].ts = sch_create_timestamp(light[i].t);
    }

    for (i = 0; i < CO2_LENGTH; i ++) {
        co2[i].ts = sch_create_timestamp(co2[i].t);
    }

    for (i = 0; i < FILTER_LENGTH; i ++) {
        filter[i].ts = sch_create_timestamp(filter[i].t);
    }
}

void sch_update_pin(int gpio, int state) {
    Serial.printf("   pin %d is set to %d\n", gpio, state);
    if (state) {
        digitalWrite(gpio, LOW);
    } else {
        digitalWrite(gpio, HIGH);
    }
}

void sch_toggle_feeding() {
    if (is_maintenance) {
        return;
    }
    if (is_feeding) {
        lcd_print(0, 0, " ", 1);
        is_feeding = 0;
        lcd_print(0,0, " ", 1);
    } else {
        is_feeding = 1;
        lcd_print(0,0, " -=* FEEDING *=-", 1);
        feeding_ts = tm_timestamp();
        light_state = 1;
        sch_update_pin(LIGHT_GPIO, light_state);
        filter_state = 0;
        sch_update_pin(FILTER_GPIO, filter_state);
        co2_state = 0;
        sch_update_pin(CO2_GPIO, co2_state);
    }
}

void sch_toggle_maintenance() {
    if (is_feeding) {
        return;
    }
    if (is_maintenance) {
        lcd_print(0, 0, " ", 1);
        is_maintenance = 0;
    } else {
        is_maintenance = 1;
        lcd_print(0, 0, "* MAINTENANCE * ", 1);
        maintenance_ts = tm_timestamp();
        light_state = 1;
        sch_update_pin(LIGHT_GPIO, light_state);
        filter_state = 0;
        sch_update_pin(FILTER_GPIO, filter_state);
        co2_state = 0;
        sch_update_pin(CO2_GPIO, co2_state);
    }
}

void sch_update_maintenance() {
    int elapsed_seconds = tm_timestamp() - maintenance_ts;
    int remaining_seconds = MAINTENANCE_TIMEOUT - elapsed_seconds;

    if (remaining_seconds <= 0) {
        sch_toggle_maintenance();
    }
    
    const int remaining_hours = remaining_seconds / (60 * 60);
    const int remaining_min = (remaining_seconds - remaining_hours * 60 * 60) / 60;
    const int remaining_sec = remaining_seconds % 60;
    char remaining_str[32];
    sprintf(remaining_str, "%02d:%02d:%02d", remaining_hours, remaining_min, remaining_sec);
    lcd_print(0, 1, remaining_str, 0);
}

void sch_update_feeding() {
    int elapsed_seconds = tm_timestamp() - feeding_ts;
    int remaining_seconds = FEEDING_TIMEOUT - elapsed_seconds;

    if (remaining_seconds <= 0) {
        sch_toggle_feeding();
    }
    
    const int remaining_hours = remaining_seconds / (60 * 60);
    const int remaining_min = (remaining_seconds - remaining_hours * 60 * 60) / 60;
    const int remaining_sec = remaining_seconds % 60;
    char remaining_str[32];
    sprintf(remaining_str, "%02d:%02d:%02d", remaining_hours, remaining_min, remaining_sec);
    lcd_print(0, 1, remaining_str, 0);
}

void sch_update(uint32_t deltaTime) {
    if (is_feeding) {
        sch_update_feeding();
        return;
    }
    if (is_maintenance) {
        sch_update_maintenance();
        return;
    }
    unsigned long timestamp = tm_day_timestamp();
    int new_light_state;
    int new_co2_state;
    int new_filter_state;

    if ((timestamp < light[0].ts) || (timestamp >= light[LIGTH_LENGTH - 1].ts)) {
        new_light_state = light[LIGTH_LENGTH - 1].state;
    } else {
        int i = 0;
        while ((i < LIGTH_LENGTH) && (timestamp >= light[i].ts)) {
            i++;
        }
        new_light_state = light[i-1].state;
    }

    if ((timestamp < co2[0].ts) || (timestamp >= co2[CO2_LENGTH - 1].ts)) {
        new_co2_state = co2[CO2_LENGTH - 1].state;
    } else {
        int i = 0;
        while ((i < CO2_LENGTH) && (timestamp >= co2[i].ts)) {
            i++;
        }
        new_co2_state = co2[i-1].state;
    }

    if ((timestamp < filter[0].ts) || (timestamp >= filter[FILTER_LENGTH - 1].ts)) {
        new_filter_state = filter[FILTER_LENGTH - 1].state;
    } else {
        int i = 0;
        while ((FILTER_LENGTH) && (timestamp >= filter[i].ts)) {
            i++;
        }
        new_filter_state = filter[i-1].state;
    }

    if (new_light_state != light_state) {
        sch_update_pin(LIGHT_GPIO, new_light_state);
        light_state = new_light_state;
    }

    if (new_co2_state != co2_state) {
        sch_update_pin(CO2_GPIO, new_co2_state);
        co2_state = new_co2_state;
    }

    if (new_filter_state != filter_state) {
        sch_update_pin(FILTER_GPIO, new_filter_state);
        filter_state = new_filter_state;
    }

    tm_display();
}


