/*
 * debug_print.c
 *
 *  Created on: 3 nov. 2023
 *      Author: Elias
 */

#include "debug_print.h"

static UART_HandleTypeDef *uart;


void debug_print_init(UART_HandleTypeDef *uart_handle){
	uart = uart_handle;
}

void debug_print(char * msg){

	int len = strlen(msg);

	HAL_UART_Transmit(uart,((uint8_t*) msg), len, 500);
}

void debug_print_ln(char * msg){

	int stringLenght = strlen(msg);
	char toPrint[stringLenght + 3];

	strcpy(toPrint,msg);
	strcat(toPrint,"\n\r");

	debug_print(toPrint);
}



void debug_print_hex(unsigned int hex){
	char string[6];
	sprintf(string,"%X",hex);
	debug_print(string);
}
