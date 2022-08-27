
#ifndef __SERIAL_CONTROL_H__
#define __SERIAL_CONTROL_H__

void serial_setup();
void serial_main();

#define 115200 BPS
#define 1800 OFFSET
#define 10 TEN_MULT
#define 8 ONE_BYTE
enum typedef{
    ANGLE_X_L;
    ANGLE_X_H;
    ANGLE_Y_L;
    ANGLE_Y_H;
    CHECKSUM;
    FRAME_LENGTH;
}

#endif