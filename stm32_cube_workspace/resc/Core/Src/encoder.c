/*
 * encoder.c
 *
 *  Created on: 2 okt. 2022
 *      Author: Elias
 */
#include "encoder.h"
#define encoder_pulses_per_rev 4095
#define PI 3.14159265359f

EncoderSignals encoder;

extern TIM_HandleTypeDef htim3;
extern I2C_HandleTypeDef hi2c1;

int encoder_calibrated = 0;
float old_angle = 0;


AS5_STATUS status;
AS5_CONF conf;


void encoder_init(){
	//HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
	set_update_rate(0x3);
	set_zero_power_mode();
	as5601_init(&hi2c1);
	read_status(&status);
	read_conf(&conf);
	old_angle = 0;
	//HAL_I2C_DeInit(&hi2c1);

}


void encoder_set_zero(){
	// TODO
	encoder_calibrated = 1;
}


void encoder_update(){

	AS5_OUTPUT out;
	float ts = 0.0005;
	//as5601_start_reading_angle();
	read_output(&out);

	uint16_t pulse_count = out.raw_angle;
	encoder.angle = pulse_count;// * 1.0/encoder_pulses_per_rev * 2*PI;
	encoder.velocity = (encoder.angle - old_angle)*1/ts;
	old_angle = encoder.angle;

	update_log_signal(encoder.angle, 0);

}





