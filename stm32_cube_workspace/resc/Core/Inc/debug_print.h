/*
 * debug_print.h
 *
 *  Created on: 3 nov. 2023
 *      Author: Elias
 */

#ifndef INC_DEBUG_PRINT_H_
#define INC_DEBUG_PRINT_H_


#include <string.h>
#include "stm32f4xx_hal.h"
#include <stdio.h>

void debug_print_init(UART_HandleTypeDef *uart_handle);
void debug_print(char * msg);
void debug_print_ln(char * msg);
void debug_print_hex(unsigned int hex);

#endif /* INC_DEBUG_PRINT_H_ */
