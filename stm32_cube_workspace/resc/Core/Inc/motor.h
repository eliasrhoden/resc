/*
 * motor.h
 *
 *  Created on: Oct 22, 2022
 *      Author: Elias
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f4xx_hal.h"
#include "encoder.h"
#include "math.h"
#include "logger.h"
#include "encoder.h"
#include "CurrentCtrl.h"


typedef struct{
	float U,V,W;
} MtrPhases;


extern int elec_ang_zero_offset;
extern MtrPhases mtr_voltages;
extern MtrPhases mtr_currents;

extern int elec_search_state2;

void mtr_init();
void mtr_set_Ud_Uq(float Ud, float Uq);
void mtr_adc_irq(ADC_HandleTypeDef* hadc);
void mtr_pwm_timer2_irq();
void mtr_pwm_timer3_irq();
void mtr_pwm_timer5_irq();
void mtr_set_U(float duty);
void mtr_set_V(float duty);
void mtr_set_W(float duty);
void mtr_search_zero();
void slave_timer_callback(TIM_HandleTypeDef *htim);
void mtr_current_ctrl_step(void);

#endif /* INC_MOTOR_H_ */
