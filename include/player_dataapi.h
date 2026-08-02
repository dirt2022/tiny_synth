#pragma once
#include <stddef.h>

struct ChainTab {
	void * data;
	struct ChainTab * next;
};

#define UnsignedFloatMod(x,y) \
	x-((int)(x/y))*y

#ifndef MIN
#define MIN(x,y) ((x>y)?y:x)
#endif

typedef struct ChainTab DIH_chaintab_t; // Data In Heap Chaintab.

int RangeFArrayLookup(float * array,float num,size_t len,size_t element_num);
int LoopFArrayLookup(float * array,float num,size_t len,size_t element_num);
struct ChainTab * ChainTabCreate(void);
void ChainTabAdd(DIH_chaintab_t * prevnode,void * data);
void ChainTabDelete(DIH_chaintab_t * prevnode);
void ChainTabDestory(DIH_chaintab_t * headnode);
void ChainTabAppend(DIH_chaintab_t ** headnode, void * data);
struct ChainTab* ChainTabHeadCosume(DIH_chaintab_t * headnode);
