#include <math.h>
#include <stdlib.h>
#include "include/player_backend.h"
#include "include/player_math.h"
#include "include/player_limit.h"

static float sintab[SINTAB_LEN+1];
static float sintab_negv[SINTAB_LEN+1];

// note sintab[SINTAB_LEN]=1.0f is just to prevent overflow. It doesn't belong to sintab.
// only used by fastsin
#define BACKEND_RATE_DIV_2 BACKEND_RATE/2
#define BACKEND_RATE_DIV_4 BACKEND_RATE/4

void sintab_init(void){
	for (unsigned int i=0;i < SINTAB_LEN;i++){
		sintab[i]=sin((float)i/(float)SINTAB_LEN*PI_DIV_2);
	}
	sintab[SINTAB_LEN]=1.0f;
	for (unsigned int i=0;i < SINTAB_LEN;i++){
		sintab_negv[i]=-sintab[i];
	}
	sintab_negv[SINTAB_LEN]=-1.0f;
return;
}

float fastsin(int x){ // note 1*BACKEND_RATE is considered to be a cycle (2*pi)
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
