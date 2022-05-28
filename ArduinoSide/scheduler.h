#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "common.h"

typedef struct _sch_counter {
    u16 counter;     // カウンタ値
    u16 interval;    // インターバル[ms]
    ONOFF execflag;  // 実行フラグ
} SCH_COUNTER;

extern SCH_COUNTER cnt_serial;
extern SCH_COUNTER cnt_led;
extern SCH_COUNTER cnt_sensor;

void scheduler_setup();
void clear_counter_flag(SCH_COUNTER *self);

#endif