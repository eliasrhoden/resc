
#ifndef TRAJ_GEN
#define TRAJ_GEN

typedef struct{
	int traj_cmd;
} TrajGen_IN;

typedef struct{
	float ref_acc;
    float ref_vel;
    float ref_pos;
} TrajGen_OUT;

#endif