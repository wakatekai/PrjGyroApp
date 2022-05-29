
#include "common.h"
#include "scheduler.h"

void setup() {
    scheduler_setup();

}

void loop() {
    scheduler_exec();
}