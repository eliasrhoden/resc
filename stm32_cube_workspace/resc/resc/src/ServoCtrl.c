
#include "ServoCtrl.h"
#include <math.h>

#define deg2rad 0.0174532925f
#define pi_23 2.09439510239f
#define sqrt_23 0.81649658092f
#define PI 3.14159265359f


// Park trafo
float park_mat[2][3];
float park_inv_mat[3][2];

// Multiturn
float old_enc = 0.0f;
int N_laps = 0;

void update_park(float theta){

    // Todo update park trafo matrices

    float sin_th0, sin_th1, sin_th2;
    float cos_th0, cos_th1, cos_th2;

    sin_th0 = sinf(theta);
    sin_th1 = sinf(theta - pi_23);
    sin_th2 = sinf(theta + pi_23);

    cos_th0 = cosf(theta);
    cos_th1 = cosf(theta - pi_23);
    cos_th2 = cosf(theta + pi_23);

    // TRAFO U,V,W -> D,Q
    park_mat[0][0] = sqrt_23*sin_th0;
    park_mat[0][1] = sqrt_23*sin_th1;
    park_mat[0][2] = sqrt_23*sin_th2;

    park_mat[1][0] = sqrt_23*cos_th0;
    park_mat[1][1] = sqrt_23*cos_th1;
    park_mat[1][2] = sqrt_23*cos_th2;


    // TRAFO D,Q -> U,V,W
    park_inv_mat[0][0] = sqrt_23*sin_th0;
    park_inv_mat[0][1] = sqrt_23*cos_th0;


    park_inv_mat[1][0] = sqrt_23*sin_th1;
    park_inv_mat[1][1] = sqrt_23*cos_th1;

    park_inv_mat[2][0] = sqrt_23*sin_th2;
    park_inv_mat[2][1] = sqrt_23*cos_th2;

}


void inv_park_trafo(float Ud, float Uq, float* U, float* V, float* W){

    *U = park_inv_mat[0][0]*Ud + park_inv_mat[0][1]*Uq;
    *V = park_inv_mat[1][0]*Ud + park_inv_mat[1][1]*Uq;
    *W = park_inv_mat[2][0]*Ud + park_inv_mat[2][1]*Uq;
}


void park_trafo(float U, float V, float W, float* D, float* Q){

    *D = park_mat[0][0]*U + park_mat[0][1]*V + park_mat[0][2]*W;
    *Q = park_mat[1][0]*U + park_mat[1][1]*V + park_mat[1][2]*W;

}


float multi_turn(float enc_angle){

    float angle = 0;

    if((enc_angle <= 1.0/3.0 *2*PI) && (old_enc >= 2.0/3.0 * 2*PI)){
        N_laps += 1;
    }
    if((enc_angle >= 2.0/3.0 *2*PI) && (old_enc <= 1.0/3.0 * 2*PI)){
        N_laps -= 1;
    }
    angle = enc_angle + N_laps*2*PI;
    old_enc = enc_angle;
    return angle;
}


void servo_ctrl(ServoCtrl_IN * in, ServoCtrl_OUT * out){

    float Ts = 0.0066*5;
    static float old_theta;

    out->rotor_ang = multi_turn(in->encoder_ang);

    out->debug0 = in->encoder_ang;
    out->debug1 = out->rotor_ang;

    out->rotor_vel = (out->rotor_ang - old_theta)/Ts;
    old_theta = out->rotor_ang;

    update_park((out->rotor_ang - 0.23317598806) *7 );

}

