#include <Keyboard.h>
#include <Wire.h>

#ifndef __SENSOR_CONTROL_H__
#define __SENSOR_CONTROL_H__

void sensor_setup();
void sensor_main();
void sensor_value_send(float,float);

// MPU-6050のアドレス、レジスタ設定値
#define MPU6050_WHO_AM_I     0x75  // Read Only
#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
#define MPU_ADDRESS   0x68

#endif