
#include "CurrentCtrl.h"
#include "ServoCtrl.h"
#include "rls.h"
#include <math.h>
#define deg2rad 0.0174532925f
#define rad2deg 1.0/deg2rad
#define pi_23 2.09439510239f
#define Ts 0.0006
#define OMEGA_rad_s 4


int elec_search_status = 0;
/*
0 = Not started
1 = Working first dir
2 = Working second dir
3 = Completed, offest saved in elec_offset_res
10 = Error
*/
float elec_offset_res = 0;
float elec_theta = 0;
float tick = 0;

float theta_pos[2], P_pos[4];
float theta_min[2], P_min[4];

void elec_offset_search(CurrCtrl_IN * in, CurrCtrl_OUT * out){

    float phi[2];
    float U,V,W;
    float P_tr_tol = 0.000001;
    int P_tol_ok;

    // Start search
    if(in->current_ctrl_cmd == 1 && elec_search_status == 0){
        elec_search_status = 1;
        tick = 0;
        reset_rls(P_pos,theta_pos,100);
        reset_rls(P_min,theta_min,100);
    }

    // Reset
    if(in->current_ctrl_cmd != 1){
        elec_search_status = 0;
        elec_theta = 0;
        tick = 0;
    }

    // Search first dir
    if(elec_search_status == 1){
        elec_theta += OMEGA_rad_s*Ts;
        tick++;

        phi[0] = in->rotor_ang;
        phi[1] = 1.0;

        if(tick*Ts > 1.5){
            rls_step(P_pos,theta_pos,phi,elec_theta);
        }

        P_tol_ok = P_pos[0] < P_tr_tol && P_pos[1] < P_tr_tol;

        if(P_tol_ok || tick*Ts > 7){
            elec_search_status = 2;
            tick = 0;
        }

        if(tick*Ts > 10){
            elec_search_status = 10;

        }
    }

    // Seach second dir
    if(elec_search_status == 2){
        elec_theta -= OMEGA_rad_s*Ts;
        tick++;

        phi[0] = in->rotor_ang;
        phi[1] = 1.0;

        if(tick*Ts > 1.5){
            rls_step(P_min,theta_min,phi,elec_theta);
        }

        P_tol_ok = P_min[0] < P_tr_tol && P_min[1] < P_tr_tol;

        if(P_tol_ok || tick*Ts > 9){
            elec_search_status = 3;

            // Compute no polepairs and enc offset

            float n_polepairs = 0.5*(theta_pos[0] + theta_min[0]);
            n_polepairs = round(n_polepairs);
            float elec_offs = 0.5*(theta_pos[1] + theta_min[1]);
            elec_offs = elec_offs * 1.0/n_polepairs;

            // Round elec offs to closest range
            float pole_div = 360.0*deg2rad/n_polepairs;
            int no_elec_periods = round(elec_offs/pole_div);
            elec_offs -= no_elec_periods*pole_div;

            //out->debug0 = elec_offs*rad2deg;
            out->debug1 = elec_offs;


        }

        if(tick*Ts > 8){
            elec_search_status = 10;
        }
    }


    //out->debug0 = theta_pos[0];
    //out->debug1 = theta_pos[1];
    //out->debug2 = theta_min[0];
    //out->debug3 = theta_min[1];


    out->debug0 = elec_theta;
    //out->debug1 = in->rotor_ang;


    if(elec_search_status== 1 || elec_search_status == 2){
        U = 0.3 + 0.05*sinf(elec_theta);
        V = 0.3 + 0.05*sinf(elec_theta - 120*deg2rad);
        W = 0.3 + 0.05*sinf(elec_theta + 120*deg2rad);

        out->phase_U = U;
        out->phase_V = V;
        out->phase_W = W;
    }


    // If done or error
    if(elec_search_status == 3 || elec_search_status == 10){
        out->phase_U = 0.5;
        out->phase_V = 0.5;
        out->phase_W = 0.5;
    }

}

float saturation(float x, float x_lim){
    if(x > x_lim){
        return x_lim;
    }

    if(x < -1*x_lim){
        return -1*x_lim;
    }

    return x;
}

void dq_pi(float ref_d, float ref_q, float y_d, float y_q, float* Ud, float* Uq){

    static float err_int_d, err_int_q;
    //float Ts = 0.006;

    float err_d, err_q;
    float Kp,Ki;

    Kp = 0.03;
    Ki = 0.09;

    err_d = ref_d - y_d;
    err_q = ref_q - y_q;


    *Ud = Kp*err_d + Ki*err_int_d;
    *Uq = Kp*err_q + Ki*err_int_q;

    *Ud = saturation(*Ud,1.0);
    *Uq = saturation(*Uq,1.0);

    if(fabs(*Ud)<1.0 && fabs(*Uq)<1.0){
        err_int_d += err_d*Ts;
        err_int_q += err_q*Ts;
    }


}

volatile void current_ctrl(CurrCtrl_IN * in, CurrCtrl_OUT * out){

    float d,q;
    float Ud,Uq;
    static int step;
    float refq;

    elec_offset_search(in,out);

    //step += 1;
    //if(step>1000){
    //    refq = 0.1337;
    //}else{
    //    refq = 0.0;
    //}

    park_trafo(in->Iu,in->Iv, in->Iw,&d,&q);

    //dq_pi(0.0,refq,d,q,&Ud,&Uq);

    //inv_park_trafo(Ud,Uq,&out->phase_U,&out->phase_V,&out->phase_W);

    out->debug0 = d;
    out->debug1 = q;

    out->debug2 = Ud;
    out->debug3 = Uq;

}

