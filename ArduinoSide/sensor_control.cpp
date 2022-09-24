#include <Arduino.h>
#include "common.h"
#include "sensor_control.h"

void sensor_setup() {
	// デバイス初期化時に実行される
	Wire.begin();

	// PCとの通信を開始
	Serial.begin(115200); //115200bps
 
	// 初回の読み出し
	Wire.beginTransmission(MPU_ADDRESS);
	Wire.write(MPU6050_WHO_AM_I);  //MPU6050_PWR_MGMT_1
	Wire.write(0x00);
	Wire.endTransmission();

	// 動作モードの読み出し
	Wire.beginTransmission(MPU_ADDRESS);
	Wire.write(MPU6050_PWR_MGMT_1);  //MPU6050_PWR_MGMT_1レジスタの設定
	Wire.write(0x00);
	Wire.endTransmission();
}

void sensor_main() {
	
	st_t sv;	//送信データセット用

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

	// 加速度値を分解能で割って加速度(G)に変換する
	float acc_x = axRaw / 16384.0;  //FS_SEL_0 16,384 LSB / g
	float acc_y = ayRaw / 16384.0;
	float acc_z = azRaw / 16384.0;

	// 角速度値を分解能で割って角速度(degrees per sec)に変換する
	float gyro_x = gxRaw / 131.0;//FS_SEL_0 131 LSB / (°/s)
	float gyro_y = gyRaw / 131.0;
	float gyro_z = gzRaw / 131.0;

	float vertical = atan(acc_x/acc_z) * (180 / M_PI);      //縦向きの操作
	float horizontal = atan(acc_y/acc_z) * (180 / M_PI);    //横向きの操作

	sv = sensor_value_send(vertical, horizontal);
}

st_t sensor_value_send(float send_vertical,float send_horizontal) {

	st_t sv;
	
	sv.vertical = send_vertical;			//縦向きの値
	sv.horizontal = send_horizontal;		//横向きの値
	
	return sv;
}
