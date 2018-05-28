#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <Arduino.h>

struct time_item {
    int hours;
    int minutes;
    int seconds;
};

struct schedule_item {
    time_item t;
    unsigned long ts;
    int state;
};

void sch_init();
void sch_update(uint32_t deltaTime);
void sch_toggle_feeding();
void sch_toggle_maintenance();

#endif
