/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

struct ChainTab {
	void * data;
	struct ChainTab * next;
};

// this isn't suitable for maths operations. but works for tick detect.
// common implement would be available in player_math.h
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
