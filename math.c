#include <math.h>
#include <stdlib.h>
#include "include/player_backend.h"
#include "include/player_math.h"
#include "include/player_limit.h"

static float sintab[SINTAB_LEN];

// only used by fastsin
static const unsigned int backend_rate_div_2=BACKEND_RATE/2;
static const unsigned int backend_rate_div_4=BACKEND_RATE/4;

void sintab_init(void){
	for (unsigned int i=0;i < SINTAB_LEN;i++){
		sintab[i]=sin((float)i/(float)SINTAB_LEN*PI_DIV_2);
	}
return;
}

float fastsin(int x){ // note 1*BACKEND_RATE is considered to be a cycle (2*pi)
	unsigned int input;

	input=x%BACKEND_RATE;
	int flip=0;
	unsigned int index;

	if (input > backend_rate_div_2){
		input-=backend_rate_div_2;
		flip=1;
	}
	if (input > backend_rate_div_4){
		input=backend_rate_div_2-input;
	}
	if (input == backend_rate_div_4){
		return flip ? -1.0f:1.0f;
	}
	index=(unsigned int)(((float)input)*((float)SINTAB_LEN)/((float)backend_rate_div_4));
	return flip? -sintab[index]:sintab[index];
}
