#include "scheduler.hpp"
#include <Arduino.h>
#include "time_manager.hpp"

int light_state = -1;
int co2_state = -1;
int filter_state = -1;

int is_feeding = 0;
int is_maintenance = 0;

const int LIGHT_GPIO = 4;
const int CO2_GPIO = 5;
const int FILTER_GPIO = 6;

const unsigned long HOURS_SEC = 60*60;
const unsigned long MINUTES_SEC = 60;

const unsigned long LIGTH_LENGTH = 4;
const unsigned long CO2_LENGTH = 2;
const unsigned long FILTER_LENGTH = 1;

schedule_item light[LIGTH_LENGTH] = {
    { { 21, 03, 0 }, 0, 1 },
    { { 21, 03, 15 }, 0, 0 },
    { { 21, 03, 30 }, 0, 1 },
    { { 21, 03, 45 }, 0, 0 }
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
}

void sch_update(uint32_t deltaTime) {
    if (is_feeding || is_maintenance) {
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
    
    char date_str[32];
    tm_time_str(date_str);
    Serial.println(" ");
    Serial.printf("[%s] light state: %d\n", date_str, light_state);
    Serial.printf("[%s] CO2 state: %d\n", date_str, co2_state);
    Serial.printf("[%s] filter state: %d\n", date_str, filter_state);
}


