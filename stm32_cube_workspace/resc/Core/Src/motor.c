/*
 * motor.c
 *
 *  Created on: Oct 22, 2022
 *      Author: Elias
 */


#include "motor.h"

#define max_timer 8400
// prev 8400

#define max_duty 0.9f
#define adc_offset 100 // 500
#define deg2rad 0.0174532925f

#define adc_1_conv_complete __HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)
#define adc_2_conv_complete __HAL_ADC_GET_FLAG(&hadc2, ADC_FLAG_EOC)
#define adc_3_conv_complete __HAL_ADC_GET_FLAG(&hadc3, ADC_FLAG_EOC)

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

uint16_t adc_U_phase;
uint16_t adc_V_phase;
uint16_t adc_W_phase;

int elec_ang_zero_offset;
float elec_ang, elec_ang_vel;
MtrPhases mtr_voltages;
MtrPhases mtr_currents;

int elec_search_state2;

extern EncoderSignals encoder;

CurrCtrl_IN cc_in;
CurrCtrl_OUT cc_out;

ServoCtrl_IN sc_in;
ServoCtrl_OUT sc_out;

TrajGen_IN tg_in;
TrajGen_OUT tg_out;

void mtr_init(){

	// MTR Voltage PWM
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

	// ADC Current measurement
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
	HAL_ADC_Start(&hadc3);

	//HAL_ADCEx_InjectedStart_IT(&hadc1);
	//HAL_ADCEx_InjectedStart_IT(&hadc2);
	//HAL_ADCEx_InjectedStart_IT(&hadc3);

	//HAL_ADC_Start_IT(&hadc1);
	//HAL_ADC_Start_IT(&hadc2);
	//HAL_ADC_Start_IT(&hadc3);

	// PWM for ADC Trigger
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
}



float limit_duty(float duty){
	if(duty > max_duty){
		duty = max_duty;
	}

	if(duty < 0.0){
		duty = 0.0f;
	}

	return duty;
}



float theta = 0;
float speed_gain = 1;
uint8_t edge = 0;


void clear_adc_regs(ADC_HandleTypeDef * adc){

	// Since we only read in the current loop, there will be some "overrun", need to clear all registers so the adc can re-trigger

	adc->Instance->SR &= ~((1<<5) | (1<<1)); // Clear OVR (b5) and EOC (b1)


}

void mtr_current_ctrl_step(void){


	// Read ADC
	adc_U_phase = HAL_ADC_GetValue(&hadc1);
	adc_V_phase = HAL_ADC_GetValue(&hadc2);
	adc_W_phase = HAL_ADC_GetValue(&hadc3);

	clear_adc_regs(&hadc1);
	clear_adc_regs(&hadc2);
	clear_adc_regs(&hadc3);


	// Convert ADC to currents
	mtr_currents.U = adc_U_phase;
	mtr_currents.V = adc_V_phase;
	mtr_currents.W = adc_W_phase;


	// Inputs
	cc_in.Iu = mtr_currents.U;
	cc_in.Iv = mtr_currents.V;
	cc_in.Iw = mtr_currents.W;

	cc_in.current_ctrl_cmd = elec_search_state2;

	cc_in.ref_Id = 0;
	cc_in.ref_Iq = 0;

	//cc_in.rotor_ang = encoder.angle;
	//cc_in.rotor_vel = encoder.velocity;
	cc_in.rotor_ang = sc_out.rotor_ang;
	cc_in.rotor_vel = sc_out.rotor_vel;

	current_ctrl(&cc_in, &cc_out);

	mtr_voltages.U = cc_out.phase_U;
	mtr_voltages.V = cc_out.phase_V;
	mtr_voltages.W = cc_out.phase_W;


	update_log_signal(mtr_currents.U, 2);
	update_log_signal(mtr_currents.V, 3);
	update_log_signal(mtr_currents.W, 4);


	update_log_signal(mtr_voltages.U, 5);
	update_log_signal(mtr_voltages.V, 6);
	update_log_signal(mtr_voltages.W, 7);

	update_log_signal(cc_out.debug0, 8);
	update_log_signal(cc_out.debug1, 9);
	//update_log_signal(cc_out.debug2, 10);
	//update_log_signal(cc_out.debug3, 11);

	// Set voltage to PWM
	mtr_set_U(mtr_voltages.U);
	mtr_set_V(mtr_voltages.V);
	mtr_set_W(mtr_voltages.W);

}



void mtr_servo_ctrl_step(void){
	encoder_update();
	sc_in.encoder_ang = encoder.angle;

	servo_ctrl(&sc_in,&sc_out);

}

void mtr_set_U(float duty){

	uint16_t duty_int = limit_duty(duty)*max_timer;

	TIM2->CCR1 = duty_int; 		// PWM
	//TIM5->SR = ~TIM_SR_CC1IF; // Clear interupt flag
	TIM5->CCR1 = TIM2->CCR1 + adc_offset;	// ADC Sampling


}

void mtr_set_V(float duty){

	uint16_t duty_int = limit_duty(duty)*max_timer;

	TIM2->CCR2 = duty_int; 		// PWM
	//TIM5->SR = ~TIM_SR_CC2IF; // Clear interupt flag
	TIM5->CCR2 = TIM2->CCR2 + adc_offset;	// ADC Sampling

}


void mtr_set_W(float duty){

	uint16_t duty_int = limit_duty(duty)*max_timer;

	TIM2->CCR3 = duty_int; 		// PWM
	//TIM5->SR = ~TIM_SR_CC3IF; // Clear interupt flag
	TIM5->CCR3 = TIM2->CCR3 + adc_offset;	// ADC Sampling
}

