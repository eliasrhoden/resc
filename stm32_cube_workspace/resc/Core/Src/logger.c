/*
 * logger.c
 *
 *  Created on: 28 juli 2022
 *      Author: Elias
 */

#include "logger.h"

//#ifdef use_old_logger

static UART_HandleTypeDef *uart;

static LogSample current_sample;

static LogSample log_buffer0[buffer_size];
static LogSample log_buffer1[buffer_size];

static LogSample *active_buffer_logz;
static int buffer_index = 0;
static uint32_t log_index = 0;

volatile uint8_t logging_state = 0;
volatile uint8_t waiting_for_host = 1;
volatile uint8_t waiting_to_send = 1;
uint8_t logging_started = 0;
uint32_t tic = 0;
uint32_t overflow_in_send=0;

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

	HAL_StatusTypeDef status;


	while(1){
		// Try to send ping
		while(1){
			status = HAL_UART_Transmit(uart, ping, 5, 500);
			if(status == HAL_OK){
				break;
			}
		}

		// See if host responds
		status = HAL_UART_Receive(uart, resp, 4, 300);
		if(status == HAL_OK){

			char msg_ok = 1;

			for (int i = 0; i < 4; i++) {
				msg_ok &= resp[i] == exp_resp[i];
			}

			if(msg_ok){
				break;
			}
		}
	}
}


void wait_for_hostold() {

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
	active_buffer_logz = log_buffer0;
	buffer_index = 0;
	log_index = 0;




	wait_for_host();

	// Send nr of floats and buffersize
	uint32_t msg[2];
	msg[0] = (int) log_nr_signals;
	msg[1] = (int) buffer_size;

	waiting_to_send = 1;
	HAL_UART_Transmit_IT(uart, (uint8_t*) msg, sizeof(msg));

	while (waiting_to_send) {

	}

	logging_started = 1;

}




void logger_step(void) {
	/*
	 * Called by a timer interupt
	 * */

	//current_sample.tic = tic;

	if(logging_started){


		if (buffer_index == 0) {
			log_buffer0[log_index].tic = tic;
		}
		else{
			log_buffer1[log_index].tic = tic;
		}

		log_index += 1;
		tic += 1;

		if (log_index >= buffer_size) {
			// Send buffer in non-blocking mode

			// Check fi previous send is completed
			if(waiting_to_send){
				overflow_in_send = 1;
			}

			waiting_to_send = 1;
			if (buffer_index == 0) {
				HAL_UART_Transmit_IT(uart, (uint8_t*) log_buffer0,
						sizeof(LogSample) * (buffer_size));

				buffer_index = 1;
			} else {

				HAL_UART_Transmit_IT(uart, (uint8_t*) log_buffer1,
						sizeof(LogSample) * (buffer_size));

				buffer_index = 0;
			}
			log_index = 0;
		}


	}



}


void logger_step2(void) {
	/*
	 * Called by a timer interupt
	 * */


	// Guess I will never know why the first LogSample in active_buffer_logz always get messed up....
	// If I disable TIM3 interrupt the problem seems to be gone
	// Will do a hard-coded- version instead just to move on
	current_sample.tic = tic;



	if(logging_started){

		memcpy(active_buffer_logz + log_index, &current_sample, sizeof(current_sample));

		log_index += 1;
		tic += 1;
	}

	if (log_index >= buffer_size) {
		// Send buffer in non-blocking mode
		//HAL_UART_Transmit_IT(uart, (uint8_t*) active_buffer_logz,
		//		sizeof(LogSample) * buffer_size);
		if (buffer_index == 0) {
			active_buffer_logz = log_buffer1;
			buffer_index = 1;
		} else {
			active_buffer_logz = log_buffer0;
			buffer_index = 0;
		}
		log_index = 0;
	}

}

void update_log_signal(float value, int index) {
	// Float is atomic so currnently there is no need
	// for a sempahore here. double -> 64bits -> two words -> not atomic!

	// Guess I don't know how pointers works after all...

	//current_sample.signals[index] = value;

	if (buffer_index == 0) {
		log_buffer0[log_index].signals[index] = value;
	}
	else{
		log_buffer1[log_index].signals[index] = value;
	}
}
//#endif

