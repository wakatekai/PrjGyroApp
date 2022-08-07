#include <Arduino.h>
#include "common.h"
//#include <Keyboard.h>
//#include <Wire.h>
// MPU-6050�̃A�h���X�A���W�X�^�ݒ�l
//#define MPU6050_WHO_AM_I     0x75  // Read Only
//#define MPU6050_PWR_MGMT_1   0x6B  // Read and Write
//#define MPU_ADDRESS  0x68

void sensor_setup() {
	// �f�o�C�X���������Ɏ��s�����
	Wire.begin();

	// PC�Ƃ̒ʐM���J�n
	Serial.begin(115200); //115200bps
 
	// ����̓ǂݏo��
	Wire.beginTransmission(MPU_ADDRESS);
	Wire.write(MPU6050_WHO_AM_I);  //MPU6050_PWR_MGMT_1
	Wire.write(0x00);
	Wire.endTransmission();

	// ���샂�[�h�̓ǂݏo��
	Wire.beginTransmission(MPU_ADDRESS);
	Wire.write(MPU6050_PWR_MGMT_1);  //MPU6050_PWR_MGMT_1���W�X�^�̐ݒ�
	Wire.write(0x00);
	Wire.endTransmission();
}

void sensor_main() {

	Wire.beginTransmission(0x68);
	Wire.write(0x3B);
	Wire.endTransmission(false);
	Wire.requestFrom(0x68, 14, true);
	while (Wire.available() < 14);
	int16_t axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw, Temperature;

	axRaw = Wire.read() << 8 | Wire.read();
	ayRaw = Wire.read() << 8 | Wire.read();
	azRaw = Wire.read() << 8 | Wire.read();
	Temperature = Wire.read() << 8 | Wire.read();
	gxRaw = Wire.read() << 8 | Wire.read();
	gyRaw = Wire.read() << 8 | Wire.read();
	gzRaw = Wire.read() << 8 | Wire.read();

	// �����x�l�𕪉�\�Ŋ����ĉ����x(G)�ɕϊ�����
	float acc_x = axRaw / 16384.0;  //FS_SEL_0 16,384 LSB / g
	float acc_y = ayRaw / 16384.0;
	float acc_z = azRaw / 16384.0;

	// �p���x�l�𕪉�\�Ŋ����Ċp���x(degrees per sec)�ɕϊ�����
	float gyro_x = gxRaw / 131.0;//FS_SEL_0 131 LSB / (��/s)
	float gyro_y = gyRaw / 131.0;
	float gyro_z = gzRaw / 131.0;

	float vertical = atan(acc_x/acc_z) * (180 / M_PI);      //�c�����̑���
	float horizontal = atan(acc_y/acc_z) * (180 / M_PI);    //�������̑���

	sensor_value_send(vertical, horizontal);
}

void sensor_value_send(float send_vertical,float send_horizontal) {


}
