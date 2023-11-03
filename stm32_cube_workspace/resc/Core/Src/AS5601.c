/*
 * AS5601.c
 *
 *  Created on: Jan 29, 2023
 *      Author: Elias
 */

#include "AS5601.h"
#include "stm32f4xx_hal.h"
#include "debug_print.h"


I2C_HandleTypeDef * i2c;
uint8_t send_buffer[4];
uint8_t rec_buffer[4];
AS5_OUTPUT output;

uint8_t state = 0;
uint8_t angle_reg = 0x0E;
int restart_count = 0;

void as5601_init(I2C_HandleTypeDef * i2c_handler){
	i2c = i2c_handler;
	set_update_rate(0xFF);
	set_zero_power_mode();
	state = 0;

}


void as5601_start_reading_angle(void){

	if(state == 0){
		// The address pointer is not incremented when reading the angle registers
		// Thus we only read the "fine angle", 2 bytes at 0xE
		HAL_I2C_Master_Transmit_IT(i2c, AS5_I2C_WRITE_ADDR, &angle_reg, 1);
		state = 1;
		restart_count = 0;
	}else{
		restart_count += 1;
		if(restart_count > 4){
			state = 0;
			restart_count = 0;
		}
	}


}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{

	// Finished sending the angle-register address, now we request to read angle value
	HAL_I2C_Master_Receive_IT(i2c, AS5_I2C_READ_ADDR, rec_buffer, 2);
	state = 2;
}




void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){

	// Raw is the first 2 bytes
	//output.raw_angle = get_int_from_2bytes(rec_buffer);
	output.raw_angle = 0;

	// Clean angle is the second 2 bytes
	output.angle = get_int_from_2bytes(rec_buffer);
	state = 0;

}


/**
 * @brief Reads the values of a register on the AS5601 over I2C
 * @param reg_addr the register address to read
 * @param nr_bytes nr of bytes to read, if more that 1 the bytes in the consecutive
 * registers are returned as well
 * @param rec_buffer pointer to recieve area
 * @returns 1 if successful, 0 otherwise
 * */
uint8_t read_bytes(uint8_t reg_addr, uint8_t nr_bytes, uint8_t * rec_buffer){

	HAL_StatusTypeDef tr_stat, rec_stat;

	// Send register that we want to read
	tr_stat = HAL_I2C_Master_Transmit(i2c, AS5_I2C_WRITE_ADDR, &reg_addr, 1, I2C_HAL_TIMEOUT);

	// Read
	rec_stat = HAL_I2C_Master_Receive(i2c, AS5_I2C_READ_ADDR, rec_buffer, nr_bytes, I2C_HAL_TIMEOUT);

	return tr_stat == HAL_OK && rec_stat == HAL_OK;
}

/**
 * Converts the 2-bytes into a uint16
 * */
uint16_t get_int_from_2bytes(uint8_t *pdata){

	return (*pdata<<8) + *(pdata+1);
}

void read_status(AS5_STATUS * status){

	uint8_t stat1,stat2,stat3;
	uint8_t status_req;


	// Status reg
	stat1 = read_bytes(0x0B, 1, &status_req);

	status->magnet_detected = (status_req & (1<<5)) > 0;
	status->magnet_to_strong = (status_req & (1<<3)) > 0;
	status->magnet_to_weak = (status_req & (1<<4)) > 0;

	// Automatic gain
	stat2 = read_bytes(0x1A,1, &status->auto_magnetic_gain);

	// Magnet magnitude
	uint8_t resp2[2];
	stat3 = read_bytes(0x1B,2, resp2);

	status->magnitude = get_int_from_2bytes(resp2);

	if(stat1 == 0 || stat2 == 0 || stat3 == 0){
		// Error handling...
	}

}

void read_output_old(AS5_OUTPUT * output){

	uint8_t stat1,stat2;
	uint8_t buffer[2];


	// raw angle
	stat1 = read_bytes(0x0C, 2, buffer);
	output->raw_angle = get_int_from_2bytes(buffer);

	// Clean angle
	stat2 = read_bytes(0x0E, 2, buffer);
	output->angle = get_int_from_2bytes(buffer);

	if(stat1 == 0 || stat2 == 0){
		// Error handling...
	}

	/*
	 * 	int val = 0;
	AS5_CONF conf;
	read_conf(&conf);
	debug_print("FF ");
	debug_print_hex(conf.fast_filter);
	debug_print(" hyst ");
	debug_print_hex(conf.hyster);
	debug_print(" upd_rat ");
	debug_print_hex(conf.output_update_rate);
	debug_print(" pow ");
	debug_print_hex(conf.power_mode);
	debug_print(" slowf ");
	debug_print_hex(conf.slow_filter);
	debug_print(" wact ");
	debug_print_hex(conf.watchdog_timer);
	debug_print_ln("");

	val =((int) output->angle*100)/100;

	sprintf(anlge_str,"%i",val);
	debug_print_ln(anlge_str);
*/

}

void read_output(AS5_OUTPUT * output_caller){

	//memcpy(output_caller, &output, sizeof(output));
	uint8_t stat1,stat2;
	uint8_t buffer[2];

	// raw angle
	stat1 = read_bytes(0x0C, 2, buffer);
	output_caller->raw_angle = get_int_from_2bytes(buffer);

	// Clean angle
	stat2 = read_bytes(0x0E, 2, buffer);
	output_caller->angle = get_int_from_2bytes(buffer);

	if(stat1 == 0 || stat2 == 0){
		// Error handling...
	}
}



void read_conf(AS5_CONF * conf){

	uint8_t tmp_stat, stat;
	uint8_t buffer[4];

	// Zero status of all reads
	stat = 0;
	/*
	debug_print("READ CONF: ");
	stat = read_bytes(0x00, 3, buffer);
	debug_print_hex(buffer[0]);
	debug_print(" ");
	debug_print_hex(buffer[1]);
	debug_print(" ");
	debug_print_hex(buffer[2]);
	debug_print_ln("");
	*/

	// Nr of zero pos writes
	conf->nr_zero_writes = buffer[0] & 0x3;

	// Zero pos
	conf->zero_pos = get_int_from_2bytes(buffer+1);

	// Slow filter
	tmp_stat = read_bytes(0x07, 3, buffer);

	/*
	debug_print("READ CONF2: ");
	debug_print_hex(buffer[0]);
	debug_print(" ");
	debug_print_hex(buffer[1]);
	debug_print(" ");
	debug_print_hex(buffer[2]);
	debug_print_ln("");
	 */
	stat |= tmp_stat << 2;
	conf->slow_filter = buffer[0] & 0x3;

	// Fast filter
	conf->fast_filter = (buffer[0] & (0x7 << 2)) >> 2;

	// Watchdog
	conf->watchdog_timer = (buffer[0] & (0x1 << 5)) > 0;

	// Powermode
	conf->power_mode = buffer[1] & 0x3;

	//Hysteresis
	conf->hyster = (buffer[1] & (0x3<<2)) >> 2;

	// ABN/Update rate
	conf->output_update_rate = buffer[2] & 0xF;

	// Push threshold
	tmp_stat = read_bytes(0x0A, 1, buffer);
	stat |= tmp_stat << 3;
	conf->push_thr = buffer[0];

	if (stat != 0xF){
		// Error handler...
	}

}




/**
 * Sets the update rate of the module
 * ABN Register, ONLY 4 bits!
 *
 * */
void set_update_rate(uint8_t rate){

	HAL_StatusTypeDef tr_stat;

	uint8_t send_buffer[2];

	// register address
	send_buffer[0] = 0x09;

	// ABN value
	send_buffer[1] = rate & 0xF;

	// Send register that we want to read
	tr_stat = HAL_I2C_Master_Transmit(i2c, AS5_I2C_WRITE_ADDR, send_buffer, 2, I2C_HAL_TIMEOUT);


	if(tr_stat != HAL_OK){
		//Error handling...
	}
}


void set_zero_power_mode(){

	uint8_t buffer;
	HAL_StatusTypeDef tr_stat;
	uint8_t send_buffer[2];

	read_bytes(0x08, 1, &buffer);


	// register address
	send_buffer[0] = 0x08;

	// ABN value
	send_buffer[1] = buffer & ~(0x3);

	// Send register that we want to read
	tr_stat = HAL_I2C_Master_Transmit(i2c, AS5_I2C_WRITE_ADDR, send_buffer, 2, I2C_HAL_TIMEOUT);


	if(tr_stat != HAL_OK){
		//Error handling...
	}


}

void write_fast_filter(uint8_t fs){



	//TODO
}




