#include <Arduino.h>
#include <MsTimer2.h>

#include "common.h"
#include "scheduler.h"
#include "serial_control.h"
#include "led_control.h"
#include "sensor_control.h"

/////////////////////////// 構造体定義 ///////////////////////////
// カウンタ構造体の定義
typedef struct _sch_counter {
    u16 counter;     // カウンタ値
    u16 interval;    // インターバル[ms]
    ONOFF execflag;  // 実行フラグ
    void (*targetfunc)(); // 実行する関数へのポインタ
} SCH_COUNTER;

/////////////////////////// 定数定義 ///////////////////////////
#define TIMER_INTERVAL  2  // タイマーインターバル[ms]

#define INTERVAL_SERIAL ((u16)20) // 必ずTIMER_INTERVALの倍数になるように設定すること
#define INTERVAL_LED    ((u16)50)
#define INTERVAL_SENSOR ((u16)20)
#define OFFSET_SERIAL   ((u16)4)
#define OFFSET_LED      ((u16)10)
#define OFFSET_SENSOR   ((u16)0)

/////////////////////////// 外部公開変数 ///////////////////////////

/////////////////////////// 内部公開変数 ///////////////////////////
static SCH_COUNTER cnt_serial;
static SCH_COUNTER cnt_led;
static SCH_COUNTER cnt_sensor;

/////////////////////////// 関数プロトタイプ宣言 ///////////////////////////
void sheduler_timer();  // Arduinoのタイマーによって周期的に起動される関数
void init_counter(SCH_COUNTER *self, u16 interval, u16 cntinit, void (*func)()); // カウンタの初期設定
void countup(SCH_COUNTER *self);  // 時間計測用カウント処理
void runjudge(SCH_COUNTER *self); // 各領域のmain関数を設定した周期ごとにに呼び出す処理

/* /////////////////////////// 関数名：scheduler_setup() ///////////////////////////
/// 引数：なし
/// 戻り値：なし
/// 実行タイミング：初期化時 -> Arduinoのsetup関数から呼び出される
/// 処理内容：
///     スケジューラーの初期化
///     各領域のsetup関数の呼び出し
//////////////////////////////////////////////////////////////////////////////////// */
void scheduler_setup() {
    serial_setup(); // シリアル通信初期化
    sensor_setup(); // センサ初期化
    led_setup();    // LED初期化
    
    // 各領域用のカウンタの初期化
    init_counter(&cnt_serial, INTERVAL_SERIAL, OFFSET_SERIAL, serial_main); // シリアル通信用
    init_counter(&cnt_led, INTERVAL_LED, OFFSET_LED, led_main);             // LED制御用
    init_counter(&cnt_sensor, INTERVAL_SENSOR, OFFSET_SENSOR, sensor_main); // ジャイロセンサ用

    // タイマーの設定と起動
    MsTimer2::set(TIMER_INTERVAL, sheduler_timer); // TIMER_INTERVALで設定した時間ごとにscheduler_main関数を呼び出す
    MsTimer2::start(); // タイマー起動
}

/* /////////////////////////// 関数名：sheduler_timer() ///////////////////////////
/// 引数：なし
/// 戻り値：なし
/// 実行タイミング：Arduinoのタイマーから周期的に呼び出される(TIMER_INTERVAL [ms])
/// 処理内容：
///     各領域のカウンタをインクリメントする処理
//////////////////////////////////////////////////////////////////////////////////// */
void sheduler_timer() {
    countup(&cnt_serial); // シリアル通信用
    countup(&cnt_led);    // LED制御用
    countup(&cnt_sensor); // センサ用
}

/* /////////////////////////// 関数名：scheduler_exec() ///////////////////////////
/// 引数：なし
/// 戻り値：なし
/// 実行タイミング：Arduinoのloop関数から呼び出される
/// 処理内容：
///     各領域のmain関数の呼び出し判定と実行
//////////////////////////////////////////////////////////////////////////////////// */
void scheduler_main() {
    runjudge(&cnt_serial);  // シリアル通信用
    runjudge(&cnt_led);     // LED制御用
    runjudge(&cnt_sensor);  // センサ用
}

/* /////////////////////////// 関数名：init_counter() ///////////////////////////
/// 引数1: カウンタ構造体
/// 引数2: 実行周期
/// 引数3: カウンタ初期値　（実行タイミングをオフセットするために用意）
/// 引数4: 実行する関数ポインタ
/// 実行タイミング：初期化時 -> scheduler_setup()から呼び出される
/// 戻り値：なし
/// 処理内容：
///     
///     各領域のmain関数の呼び出し判定と実行
//////////////////////////////////////////////////////////////////////////////////// */
void init_counter(SCH_COUNTER *self, u16 interval, u16 cntinit, void (*func)()) {
    self->counter = cntinit;    // カウンタ初期値の設定
    self->execflag = OFF;       // 実行フラグ初期化
    self->interval = interval;  // 実行周期の設定
    self->targetfunc = func;    // 関数ポインタの登録
}

/* /////////////////////////// 関数名：countup() ///////////////////////////
/// 引数1: カウンタ構造体
/// 戻り値：なし
/// 実行タイミング：タイマー -> sheduler_timer関数から呼び出される
/// 処理内容：
///     カウンタをインクリメントして時間を計測する
///     設定されている制御周期が来たら実行フラグをONにする
//////////////////////////////////////////////////////////////////////////////////// */
void countup(SCH_COUNTER *self) {
    // 実行フラグがOFF かつ カウンタ値 < 制御周期 の時カウントアップしていく
    if (self->execflag == OFF && self->counter < self->interval) {
        self->counter += TIMER_INTERVAL; // タイマー設定値ずつカウントアップ
    }
    // カウント値が制御周期に達したら
    if (self->counter >= self->interval) {
        self->counter = 0; // カウンタをリセット
        self->execflag = ON; // 実行フラグをONにする
    }
}

/* /////////////////////////// 関数名：runjudge() ///////////////////////////
/// 引数1: カウンタ構造体
/// 戻り値：なし
/// 実行タイミング：Arduinoのloop関数実行タイミング -> scheduler_main関数から呼び出される
/// 処理内容：
///     カウンタをインクリメントして時間を計測する
///     設定されている制御周期が来たら実行フラグをONにする
//////////////////////////////////////////////////////////////////////////////////// */
void runjudge(SCH_COUNTER *self) {
    // 実行フラグがONだったら
    if (self->execflag == ON) {
        self->targetfunc(); // 登録してある関数を実行する
        self->execflag = OFF; // 実行フラグをクリア
    }
}
