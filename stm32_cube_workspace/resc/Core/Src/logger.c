/*
 * logger.c
 *
 *  Created on: 28 juli 2022
 *      Author: Elias
 */


#include "logger.h"
#define uart_timeout 500/portTICK_RATE_MS

UART_HandleTypeDef * uart;
osSemaphoreId_t * logger_sem;

//float log_signals[log_nr_signals];

LogSample current_sample;

LogSample log_buffer0[buffer_size];
LogSample log_buffer1[buffer_size];

LogSample * active_buffer;
int buffer_index = 0;
int log_index = 0;


volatile uint8_t waiting_for_host = 1;
volatile uint8_t waiting_to_send = 1;
uint8_t logging_started = 0;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	waiting_for_host = 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	waiting_to_send = 0;
}

/*
 * Tries to establish a initalization with the host.
 *
 * */
void wait_for_host(){

	char ping[5] = "glenn";
	uint8_t resp[10];
	char exp_resp[4] = "hepp";

	// Rec from host
	waiting_for_host = 1;
	HAL_UART_Receive_IT (uart,(uint8_t*)  resp, 4);

	while(waiting_for_host){
		vTaskDelay(5.0/portTICK_RATE_MS);
	}

	char msg_ok = 1;

	for(int i=0;i<4;i++){
		msg_ok &= resp[i] == exp_resp[i];
	}

	if(msg_ok){
		waiting_to_send = 1;
		HAL_UART_Transmit_IT(uart, (uint8_t*) ping, 5);

		while(waiting_to_send){
			vTaskDelay(1.0/portTICK_RATE_MS);
		}
	}


}




void logger_task(UART_HandleTypeDef* uart_handle){


	uart = uart_handle;

	wait_for_host();

	// Send nr of floats
	uint32_t msg_len = (int) log_nr_signals;
	HAL_UART_Transmit(uart, (uint8_t*) &msg_len, 4, uart_timeout);

	// Send buffersize
	msg_len = (int) buffer_size;
	HAL_UART_Transmit(uart, (uint8_t*) &msg_len, 4, uart_timeout);

	uint32_t tick_timestamp;

	active_buffer = log_buffer0;
	buffer_index = 0;
	logging_started = 1;

	for(;;){

		// Save values to buffer
		tick_timestamp =  osKernelGetTickCount() *  portTICK_RATE_MS;
		current_sample.tic = tick_timestamp;

		memcpy(active_buffer + log_index, &current_sample, sizeof(LogSample));

		log_index += 1;

		if(log_index >= buffer_size){
			// Send buffer in non-blocking mode
			HAL_UART_Transmit_IT(uart, (uint8_t*) active_buffer, sizeof(LogSample)*buffer_size);
			if(buffer_index == 0){
				active_buffer = log_buffer1;
				buffer_index = 1;
			}else{
				active_buffer = log_buffer0;
				buffer_index = 0;
			}
			log_index = 0;
		}
		// LOGGING RATE
		vTaskDelay(8.0/portTICK_RATE_MS); // 4 MINIMUM
	}
}


void update_log_signal(float value, int index){
	// Float is atomic so currnently there is no need
	// for a sempahore here. double -> 64bits -> two words -> not atomic!
	current_sample.signals[index] = value;

}





