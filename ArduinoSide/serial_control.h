#ifndef __SERIAL_CONTROL_H__
#define __SERIAL_CONTROL_H__

void serial_setup();
void serial_main();

#define BPS 115200
#define OFFSET 1800
#define TEN_MULT 10
#define ONE_BYTE 8
typedef enum{
    ANGLE_X_H = 0,
    ANGLE_X_L,
    ANGLE_Y_H,
    ANGLE_Y_L,
    CHECKSUM,
    FRAME_LENGTH,
}ANGLE_ARRAY;

#endif