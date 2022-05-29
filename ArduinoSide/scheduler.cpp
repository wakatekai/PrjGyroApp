#include <Arduino.h>
#include <MsTimer2.h>
#include "common.h"
#include "scheduler.h"

#include "serial_control.h"
#include "led_control.h"
#include "sensor_control.h"

#define TIMER_INTERVAL  10  // タイマーインターバル[ms]

#define INTERVAL_SERIAL ((u16)100) // 必ずTIMER_INTERVALの倍数になるように設定すること
#define INTERVAL_LED    ((u16)200)
#define INTERVAL_SENSOR ((u16)200)
#define OFFSET_SERIAL   ((u16)0)
#define OFFSET_LED      ((u16)0)
#define OFFSET_SENSOR   ((u16)0)

SCH_COUNTER cnt_serial;
SCH_COUNTER cnt_led;
SCH_COUNTER cnt_sensor;

void sheduler_main();   // タイマーイベントで呼び出す関数

void init_counter(SCH_COUNTER *self, u16 interval, u16 cntinit, void (*func)());
void countup(SCH_COUNTER *self);
void runjudge(SCH_COUNTER *self);

void scheduler_setup() {
    serial_setup();
    sensor_setup();
    led_setup();

    init_counter(&cnt_serial, INTERVAL_SERIAL, OFFSET_SERIAL, serial_main);
    init_counter(&cnt_led, INTERVAL_LED, OFFSET_LED, led_main);
    init_counter(&cnt_sensor, INTERVAL_SENSOR, OFFSET_SENSOR, sensor_main);

    MsTimer2::set(TIMER_INTERVAL, sheduler_main);
    MsTimer2::start();
}

void sheduler_main() {
    countup(&cnt_serial);
    countup(&cnt_led);
    countup(&cnt_sensor);
}

void scheduler_exec() {
    runjudge(&cnt_serial);
    runjudge(&cnt_led);
    runjudge(&cnt_sensor);
}

void init_counter(SCH_COUNTER *self, u16 interval, u16 cntinit, void (*func)()) {
    self->counter = cntinit;
    self->execflag = OFF;
    self->interval = interval;
    self->targetfunc = func;
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

void runjudge(SCH_COUNTER *self) {
    if (self->execflag == ON) {
        self->targetfunc();
        self->execflag = OFF;
    }
}

void clear_counter_flag(SCH_COUNTER *self) {
    self->execflag = OFF;
}