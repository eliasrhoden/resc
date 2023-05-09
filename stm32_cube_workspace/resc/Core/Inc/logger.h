/*
 * logger.h
 *
 *  Created on: 28 juli 2022
 *      Author: Elias
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#define log_nr_signals 15
#define buffer_size 200

typedef struct log_sample{
	uint32_t tic;
	float signals[log_nr_signals];
} LogSample;



void logger_send_task(UART_HandleTypeDef* uart_handle);
void logger_task(UART_HandleTypeDef* uart_handle);
void update_log_signal(float value, int index);
void update_log_signals(float * values);

#endif /* INC_LOGGER_H_ */
