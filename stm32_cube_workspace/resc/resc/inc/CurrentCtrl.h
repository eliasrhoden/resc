
#ifndef CURR_CTRL
#define CURR_CTRL

#include "ServoCtrl.h"

typedef struct{
	float ref_Id,ref_Iq;
	float rotor_ang;
	float rotor_vel;
	int current_ctrl_cmd;
	float Iu,Iv,Iw;
} CurrCtrl_IN;

typedef struct{
	float phase_U, phase_V, phase_W;
	float debug0,debug1,debug2,debug3;
} CurrCtrl_OUT;


volatile void current_ctrl(CurrCtrl_IN * in, CurrCtrl_OUT * out);

#endif
