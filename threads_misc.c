/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "include/player_thread.h"
#include "include/player_memmacro.h"
#include "include/player_math.h"

#ifdef _OPENMP
#include <omp.h>
struct timespec * thread_speedtab=NULL;
unsigned short * thread_weight=NULL;
unsigned int * thread_pendinglen=NULL;
unsigned int * thread_wrote_offset=NULL;
#endif

#define TEST_SAMPLES 409600

#ifdef _OPENMP
unsigned int thread_num;
void Init_LoadbalanceTab(void){
	SAFE_CALLOC_DEF(thread_speedtab,thread_num,sizeof(struct timespec));
	SAFE_CALLOC_DEF(thread_weight,thread_num,sizeof(unsigned short));
	SAFE_CALLOC_DEF(thread_pendinglen,thread_num,sizeof(unsigned int));
	SAFE_CALLOC_DEF(thread_wrote_offset,thread_num,sizeof(unsigned int));
}
void Speed_Check(void){
	struct timespec start, end;
	float op;
	int flag=0;
	unsigned int id=omp_get_thread_num();
	clock_gettime(CLOCK_MONOTONIC, &start);
	if(start.tv_sec == 0){
		start.tv_sec=1;
		flag=1;
	}
	op=start.tv_sec;
	for(unsigned int i=0;i < TEST_SAMPLES; i++){
		op=fastsin(i+start.tv_sec);
		op*=fastsin(i+start.tv_nsec);
		op=op*1000000/start.tv_sec;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	if(flag){
		end.tv_sec++;
	}
	thread_speedtab[id].tv_sec=end.tv_sec-start.tv_sec;
	thread_speedtab[id].tv_nsec=end.tv_nsec-start.tv_nsec;
}
void Calc_Weight(void){
	double vsum=0.0;
	unsigned short weight_sum=0;
	for (unsigned int i=0;i<thread_num;i++){
		vsum+=1.0/(thread_speedtab[i].tv_sec*1000000000+thread_speedtab[i].tv_nsec);
	}
	for (unsigned int i=0;i<thread_num;i++){
		thread_weight[i]=THREAD_WEIGHT_SUM/vsum/(thread_speedtab[i].tv_sec*1000000000+thread_speedtab[i].tv_nsec);
		weight_sum+=thread_weight[i];
	}
	if (weight_sum != THREAD_WEIGHT_SUM){
		thread_weight[0]+=THREAD_WEIGHT_SUM-weight_sum;
	}
	printf("result:\n");
	for (unsigned int i=0;i<thread_num;i++){
		printf("tid: %d weight: %d rawtime %ld\n",i,thread_weight[i],(thread_speedtab[i].tv_sec*1000000000+thread_speedtab[i].tv_nsec));
	}
}
void DeInit_LoadbalanceTab(void){
	SAFE_FREE(thread_speedtab);
	SAFE_FREE(thread_weight);
	SAFE_FREE(thread_pendinglen);
	SAFE_FREE(thread_wrote_offset);
}
#endif
