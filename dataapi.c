/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "include/player_dataapi.h"
#include "include/player_memmacro.h"

int RangeFArrayLookup(float* array, float num, size_t len, size_t element_num) {
	if (len % element_num != 0 || element_num < 3) {
		return 0;
	}
	for (size_t i = 0; i < len; i += element_num) {
		if (num >= array[i] && num <= array[i + 1]) {
			return i;
		}
	}
	return 0;
}

int LoopFArrayLookup(float* array, float num, size_t len, size_t element_num) {
	if (len % element_num != 0 || element_num < 3) {
		return 0;
	}
	float whole_loopcycle = array[len - element_num + 1];
	for (size_t i = 0; i < len; i += element_num) {
		if (UnsignedFloatMod(num, whole_loopcycle) >= array[i] &&
		    UnsignedFloatMod(num, whole_loopcycle) <= array[i + 1]) {
			return i;
		}
	}
	return 0;
}

struct ChainTab* ChainTabCreate(void) {
	struct ChainTab* ptr = NULL;
	SAFE_MALLOC_DEF(ptr, sizeof(struct ChainTab));
	memset(ptr, 0, sizeof(struct ChainTab));
	return ptr;
}

void ChainTabAdd(struct ChainTab* prevnode, void* data) {
	struct ChainTab* ptr = NULL;
	SAFE_MALLOC_DEF(ptr, sizeof(struct ChainTab));
	ptr->data = data;
	ptr->next = prevnode->next;
	prevnode->next = ptr;
}

void ChainTabDelete(struct ChainTab* prevnode) {
	struct ChainTab* ptr = prevnode->next;
	struct ChainTab* ptnext;
	if (ptr == NULL) {
		return;
	}
	ptnext = ptr->next;
	SAFE_FREE(ptr->data);
	SAFE_FREE(ptr);
	prevnode->next = ptnext;
}

void ChainTabDestory(struct ChainTab* headnode) {
	struct ChainTab* next = NULL;
	while (headnode != NULL) {
		next = headnode->next;
		SAFE_FREE(headnode->data);
		SAFE_FREE(headnode);
		headnode = next;
	}
}
/**
如果表头有数据,就向后追加,如果没有,就直接往这个节点写数据
**/
void ChainTabAppend(struct ChainTab** headnode, void* data) {
	if (*headnode == NULL) {
		*headnode = ChainTabCreate();
		(*headnode)->data = data;
		return;
	}
	if ((*headnode)->data == NULL) { // 此时不新开元素,就在此节点写数据
		(*headnode)->data = data;
		return;
	}
	while ((*headnode)->next != NULL) {
		*headnode = (*headnode)->next;
	}
	ChainTabAdd(*headnode, data);
}

struct ChainTab* ChainTabHeadCosume(struct ChainTab* headnode) {
	struct ChainTab* next = headnode->next;
	SAFE_FREE(headnode->data);
	SAFE_FREE(headnode);
	return next;
}
