#include <Arduino.h>
#include "common.h"

#define SRCLK   (5)
#define RCLK    (6)
#define SER     (7)

// 点灯パターン(1=点灯, 0=消灯)
const uint8_t PATTERNS[] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000,
};


void led_setup() {
    pinMode(SRCLK, OUTPUT);
    pinMode(RCLK,  OUTPUT);
    pinMode(SER,   OUTPUT);
}

void led_main() {
    // 点灯パターンの数
    static uint8_t i = 0;
    int max_pattern = sizeof(PATTERNS)/sizeof(PATTERNS[0]);

    // 8ビット分のデータをシフトレジスタへ送る
    shiftOut(SER, SRCLK, LSBFIRST, PATTERNS[i]); 

    // シフトレジスタの状態をストレージレジスタへ反映させる
    digitalWrite(RCLK,  LOW);
    digitalWrite(RCLK,  HIGH);

    i++;
    i = i & 0x07;
}