/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <math.h>
#include <stdlib.h>
#include "include/player_backend.h"
#include "include/player_math.h"
#include "include/player_limit.h"

float sintab[SINTAB_LEN+1];
float sintab_negv[SINTAB_LEN+1];

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
