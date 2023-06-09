#ifndef SERVO_CTRL
#define SERVO_CTRL


void inv_park_trafo(float Ud, float Uq, float* U, float* V, float* W);
void park_trafo(float U, float V, float W, float* D, float* Q);

typedef struct{
	float encoder_ang;
    float ref_acc, ref_vel, ref_pos;
	int servo_ctrl_cmd;
} ServoCtrl_IN;

typedef struct{
	float ref_Id, ref_Iq;
    float rotor_ang, rotor_vel;
	float debug0, debug1, debug2, debug3, debug4;
} ServoCtrl_OUT;

#endif
