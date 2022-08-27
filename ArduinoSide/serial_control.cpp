#include <Arduino.h>
#include "common.h"

void serial_setup() {
  // PC�Ƃ̒ʐM���J�n
  Serial.begin(BPS); //115200bps  
}

void serial_main() {
    float Angle_y_old,Angle_x_old;
    unsigned short Angle_y,Angle_x;
    unsigned char senddate[5];

    //�^�ϊ�����10�{���ăI�t�Z�b�g���鏈��
    //�I�t�Z�b�g��-180~180����0~3600�ɁB�I�[�o�[�t���[���Ȃ����߁B
    //���10�{���Ă��琮���ɕϊ����Ȃ��ƁA�Ⴆ��179.9���󂯎�����ꍇ�ɐ�ɐ����ɕϊ������180��179�ɂȂ��Ă���TEN_MULT�{�����1799�ɂȂ�Ȃ����߁B
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

    //�`�F�b�N�T��
    for(int i = ANGLE_X_L; i < FRAME_LENGTH; i++){
      senddate[CHECKSUM] += senddate[i];
    }

    //���M�f�[�^
    Serial.Write(senddate,FRAME_LENGTH);

    return 0;
}

