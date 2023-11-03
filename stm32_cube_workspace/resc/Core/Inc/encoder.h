/*
 * encoder.h
 *
 *  Created on: 2 okt. 2022
 *      Author: Elias
 */

#ifndef INC_ENCODER
#define INC_ENCODER

#include "stm32f4xx_hal.h"
#include "logger.h"
#include "AS5601.h"
#include "debug_print.h"

typedef struct encoder{
	float angle;
	float velocity;
} EncoderSignals;

extern EncoderSignals encoder;
extern int encoder_calibrated;

void encoder_init();
void encoder_task();
void encoder_update();
void encoder_set_zero();


#endif /* INC_ENCODER_H_ */
