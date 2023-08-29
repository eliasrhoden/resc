/*
 * logger.c
 *
 *  Created on: 28 juli 2022
 *      Author: Elias
 */

#include "logger.h"

//#ifdef use_old_logger

UART_HandleTypeDef *uart;

LogSample current_sample;

LogSample log_buffer0[buffer_size];
LogSample log_buffer1[buffer_size];

LogSample *active_buffer;
int buffer_index = 0;
int log_index = 0;

volatile uint8_t logging_state = 0;
volatile uint8_t waiting_for_host = 1;
volatile uint8_t waiting_to_send = 1;
uint8_t logging_started = 0;
uint32_t tic = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	waiting_for_host = 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	waiting_to_send = 0;
}

/*
 * Tries to establish a initalization with the host.
 *
 * */
void wait_for_host() {

	char ping[5] = "glenn";
	uint8_t resp[10];
	char exp_resp[4] = "hepp";

	// Rec from host
	waiting_for_host = 1;
	HAL_UART_Receive_IT(uart, (uint8_t*) resp, 4);

	while (waiting_for_host) {
		//vTaskDelay(5.0/portTICK_RATE_MS);
	}

	char msg_ok = 1;

	for (int i = 0; i < 4; i++) {
		msg_ok &= resp[i] == exp_resp[i];
	}

	if (msg_ok) {
		waiting_to_send = 1;
		HAL_UART_Transmit_IT(uart, (uint8_t*) ping, 5);

		while (waiting_to_send) {

		}
	}

}

void logger_init(UART_HandleTypeDef *uart_handle) {

	uart = uart_handle;

	wait_for_host();

	// Send nr of floats and buffersize
	uint32_t msg[2];
	msg[0] = (int) log_nr_signals;
	msg[1] = (int) buffer_size;

	waiting_to_send = 1;
	HAL_UART_Transmit_IT(uart, (uint8_t*) msg, sizeof(msg));

	while (waiting_to_send) {

	}

	uint32_t tick_timestamp;

	active_buffer = log_buffer0;
	buffer_index = 0;
	logging_started = 1;

}

void logger_step(void) {
	/*
	 * Called by a timer interupt
	 * */

	current_sample.tic = tic;

	memcpy(active_buffer + log_index, &current_sample, sizeof(LogSample));

	if(logging_started){
		log_index += 1;
		tic += 1;
	}

	if (log_index >= buffer_size) {
		// Send buffer in non-blocking mode
		HAL_UART_Transmit_IT(uart, (uint8_t*) active_buffer,
				sizeof(LogSample) * buffer_size);
		if (buffer_index == 0) {
			active_buffer = log_buffer1;
			buffer_index = 1;
		} else {
			active_buffer = log_buffer0;
			buffer_index = 0;
		}
		log_index = 0;
	}

}

void update_log_signal(float value, int index) {
	// Float is atomic so currnently there is no need
	// for a sempahore here. double -> 64bits -> two words -> not atomic!
	current_sample.signals[index] = value;

}
//#endif

