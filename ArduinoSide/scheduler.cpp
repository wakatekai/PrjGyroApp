#include <Arduino.h>
#include <MsTimer2.h>
#include "common.h"
#include "scheduler.h"

#define TIMER_INTERVAL  10  // タイマーインターバル[ms]

#define INTERVAL_SERIAL ((u16)100)
#define INTERVAL_LED    ((u16)200)
#define INTERVAL_SENSOR ((u16)200)

SCH_COUNTER cnt_serial;
SCH_COUNTER cnt_led;
SCH_COUNTER cnt_sensor;

void sheduler_main();   // タイマーイベントで呼び出す関数

void init_counter(SCH_COUNTER *self, u16 interval);
void countup(SCH_COUNTER *self);

void scheduler_setup() {
    init_counter(&cnt_serial, INTERVAL_SERIAL);
    init_counter(&cnt_led, INTERVAL_LED);
    init_counter(&cnt_sensor, INTERVAL_SENSOR);

    MsTimer2::set(TIMER_INTERVAL, sheduler_main);
    MsTimer2::start();
}

void sheduler_main() {
    countup(&cnt_serial);
    countup(&cnt_led);
    countup(&cnt_sensor);
}

void init_counter(SCH_COUNTER *self, u16 interval) {
    self->counter = 0;
    self->execflag = OFF;
    self->interval = interval;
}

void countup(SCH_COUNTER *self) {
    if (self->execflag == OFF && self->counter < self->interval) {
        self->counter += TIMER_INTERVAL;
    }

    if (self->counter >= self->interval) {
        self->counter = 0;
        self->execflag = ON;
    }
}

void clear_counter_flag(SCH_COUNTER *self) {
    self->execflag = OFF;
}