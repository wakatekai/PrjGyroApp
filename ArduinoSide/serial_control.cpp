#include <Arduino.h>
#include "common.h"

void serial_setup() {
  // PCとの通信を開始
  Serial.begin(BPS); //115200bps  
}

void serial_main() {
    float Angle_y_old,Angle_x_old;
    unsigned short Angle_y,Angle_x;
    unsigned char senddate[5];

    //型変換して10倍してオフセットする処理
    //オフセットで-180~180から0~3600に。オーバーフローしないため。
    //先に10倍してから整数に変換しないと、例えば179.9を受け取った場合に先に整数に変換されて180や179になってからTEN_MULT倍すると1799にならないため。
    Angle_x_old = sv.horizontal;
    Angle_x_old = (Angle_x_old * TEN_MULT) + OFFSET;
    Angle_x = (unsigned short)Angle_x_old;
    senddate[ANGLE_X_L] = (unsigned char)Angle_x;
    senddate[ANGLE_X_H] = (unsigned char)(Angle_x >> ONE_BYTE);

    Angle_y_old = sv.vertical;
    Angle_y_old = (Angle_y_old * TEN_MULT) + OFFSET;
    Angle_y = (unsigned short)Angle_y_old;
    senddate[ANGLE_Y_L] = (unsigned char)Angle_y;
    senddate[ANGLE_Y_H] = (unsigned char)(Angle_y >> ONE_BYTE);

    //チェックサム
    for(int i = ANGLE_X_L; i < FRAME_LENGTH; i++){
      senddate[CHECKSUM] += senddate[i];
    }

    //送信データ
    Serial.Write(senddate,FRAME_LENGTH);

    return 0;
}

