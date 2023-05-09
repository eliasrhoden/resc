#ifndef RLS_HEADER
#define RLS_HEADER

void rls_step(float* P, float* theta, float* phi, float y);
void reset_rls(float* P, float* theta, float Pinit);

#endif
