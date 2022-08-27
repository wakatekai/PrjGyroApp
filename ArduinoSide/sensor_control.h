#include <Keyboard.h>
#include <Wire.h>

#ifndef __SENSOR_CONTROL_H__
#define __SENSOR_CONTROL_H__


typedef struct{
    float vertical;   //�c�����̑��� -180�`180��
    float horizontal; //�������̑��� -180�`180��
}st_t;


void sensor_setup();
void sensor_main();
st_t sensor_value_send(float,float);


// MPU-6050�̃A�h���X�A���W�X�^�ݒ�l
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS   0x68

#endif