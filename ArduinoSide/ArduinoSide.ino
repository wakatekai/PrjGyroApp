
#include "common.h"
#include "scheduler.h"
#include "serial_control.h"
#include "led_control.h"
#include "sensor_control.h"

void setup() {
    scheduler_setup();
    serial_setup();
    sensor_setup();
    led_setup();
}

void loop() {
    if (cnt_sensor.execflag == ON) {
        sensor_main();
        clear_counter_flag(&cnt_sensor);
    }
    
    if (cnt_serial.execflag == ON) {
        serial_main();
        clear_counter_flag(&cnt_serial);
    }
    
    if (cnt_led.execflag == ON) {
        led_main();
        clear_counter_flag(&cnt_led);
    }
}