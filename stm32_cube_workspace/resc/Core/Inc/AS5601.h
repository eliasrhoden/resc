/*
 * AS5601.h
 *
 * HAL Effect angle sensor
 *
 *  Created on: Jan 29, 2023
 *      Author: Elias
 */

#ifndef INC_AS5601_H_
#define INC_AS5601_H_

//#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "string.h"

#define AS5_I2C_ADDR 0x36
#define AS5_I2C_READ_ADDR (AS5_I2C_ADDR<<1) | 0x1
#define AS5_I2C_WRITE_ADDR (AS5_I2C_ADDR<<1)
#define I2C_HAL_TIMEOUT 1000

typedef struct{
	uint8_t nr_zero_writes;
	uint16_t zero_pos;
	uint8_t slow_filter;
	uint8_t hyster;
	uint8_t fast_filter;
	uint8_t watchdog_timer;
	uint8_t power_mode;
	uint8_t output_update_rate;
	uint8_t push_thr;
} AS5_CONF;

typedef struct{
	uint16_t raw_angle; // Unscaled
	uint16_t angle;	// Scaled, filtered + Hysteresis
} AS5_OUTPUT;

typedef struct{
	uint8_t magnet_to_strong;
	uint8_t magnet_to_weak;
	uint8_t magnet_detected;
	uint8_t auto_magnetic_gain;
	uint16_t magnitude;
} AS5_STATUS;



void as5601_init(I2C_HandleTypeDef * i2c_handler);
void as5601_start_reading_angle(void);
void read_status(AS5_STATUS * status);
void read_output(AS5_OUTPUT * output);
void read_conf(AS5_CONF * conf);
void set_zero_power_mode();
void set_update_rate(uint8_t rate);
uint16_t get_int_from_2bytes(uint8_t *pdata);

#endif /* INC_AS5601_H_ */
