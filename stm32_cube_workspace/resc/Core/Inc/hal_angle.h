/*
 * hal_angle.h
 *
 *  Created on: Jan 29, 2023
 *      Author: Elias
 */

#ifndef INC_HAL_ANGLE_H_
#define INC_HAL_ANGLE_H_

#include "stm32f4xx_hal.h"

void sensor_init(void);

uint8_t sensor_ok(void);


#endif /* INC_HAL_ANGLE_H_ */
