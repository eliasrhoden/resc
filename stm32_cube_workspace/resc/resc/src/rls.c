
#include "rls.h"

void inv_2_mat(float* mat, float* res){
    /* Inverse of 2x2 matrix*/

    float determ;

    determ = mat[0]*mat[3] - mat[1]*mat[2];

    res[0] = mat[3]*1/determ;
    res[1] = -1*mat[1]*1/determ;
    res[2] = -1*mat[2]*1/determ;
    res[3] = mat[0]*1/determ;
}

void mat_times_vec2(float* mat, float* vec, float* res){
    /* 2x2 matrix times a 2x1 vector*/

    res[0] = mat[0]*vec[0] + mat[1]*vec[1];
    res[1] = mat[2]*vec[0] + mat[3]*vec[1];
}

void mat_times_mat2(float*mat1, float*mat2, float* res){
    /* 2x2 matrix times a 2x2 matrix*/

    res[0] = mat1[0]*mat2[0] + mat1[1]*mat2[2];
    res[1] = mat1[0]*mat2[1] + mat1[1]*mat2[3];

    res[2] = mat1[2]*mat2[0] + mat1[3]*mat2[1];
    res[3] = mat1[2]*mat2[1] + mat1[3]*mat2[3];

}


void compute_K2(float* P, float* phi, float lam, float* K){
    /* Compuites the "Kalman gain" used in RLS*/

    mat_times_vec2(P,phi,K);

    float gain;

    gain = lam + phi[0]*K[0] + phi[1]*K[1];

    K[0] *= 1/gain;
    K[1] *= 1/gain;
}


void compute_P(float* K,float* phi, float* P, float lam, float* P_res){
    /* Computes the covariance matrix P*/

    float tmp_mat[4];

    tmp_mat[0] = 1 - K[0]*phi[0];
    tmp_mat[1] = - K[0]*phi[1];

    tmp_mat[2] = - K[1]*phi[0];
    tmp_mat[3] = 1 - K[1]*phi[1];

    mat_times_mat2(tmp_mat,P,P_res);

    P_res[0] *= 1/lam;
    P_res[1] *= 1/lam;
    P_res[2] *= 1/lam;
    P_res[3] *= 1/lam;
}

void compute_theta(float* K, float* phi, float* theta, float y, float* theta_plus){
    /* Computes the updated theta vecotr*/

    float eps;

    eps = y - (phi[0]*theta[0] + phi[1]*theta[1]);

    theta_plus[0] = theta[0] + K[0]*eps;
    theta_plus[1] = theta[1] + K[1]*eps;
}


void rls_step(float* P, float* theta, float* phi, float y){
    float K[2];
    float lam = 0.999;
    float theta_plus[2];
    float Pp[4];

    compute_K2(P,phi,lam,K);
    compute_P(K,phi,P,lam,Pp);
    compute_theta(K,phi,theta,y,theta_plus);

    theta[0] = theta_plus[0];
    theta[1] = theta_plus[1];

    P[0] = Pp[0];
    P[1] = Pp[1];
    P[2] = Pp[2];
    P[3] = Pp[3];
}

void reset_rls(float* P, float* theta, float Pinit){
    P[0] = Pinit;
    P[3] = Pinit;
    P[1] = 0;
    P[2] = 0;

    theta[0] = 0;
    theta[1] = 0;
}

