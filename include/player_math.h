#pragma once
#include "player_limit.h"
// note sintab[SINTAB_LEN]=1.0f is just to prevent overflow. It doesn't belong to sintab.
#define BACKEND_RATE_DIV_2 BACKEND_RATE/2
#define BACKEND_RATE_DIV_4 BACKEND_RATE/4

#define PI 3.141592653589793
#define PI_DIV_2 PI/2
#define PI_CROSS_2 PI*2

extern float sintab[];
extern float sintab_negv[];

inline static float fastsin(int x);
void sintab_init(void);

inline static float fastsin(int x){ // note 1*BACKEND_RATE is considered to be a cycle (2*pi)
	x=x%BACKEND_RATE;
	int flip=0;
	unsigned int index;

	if (x > BACKEND_RATE_DIV_2){
		x-=BACKEND_RATE_DIV_2;
		flip=1;
	}
	if (x > BACKEND_RATE_DIV_4){
		x=BACKEND_RATE_DIV_2-x;
	}
	index=(unsigned int)(((float)x)*((float)SINTAB_LEN)/((float)BACKEND_RATE_DIV_4));
	return flip? sintab_negv[index]:sintab[index];
}

