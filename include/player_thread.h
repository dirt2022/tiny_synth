/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef _OPENMP
#define THREAD_WEIGHT_SUM 1000
#define THREAD_OFFLOAD_MIN_THRESHOLD 256
extern unsigned int thread_num;
extern struct timespec* thread_speedtab;
extern unsigned short* thread_weight;
extern unsigned int* thread_pendinglen;
extern unsigned int* thread_wrote_offset;
void Calc_Weight(void);
void Init_LoadbalanceTab(void);
void DeInit_LoadbalanceTab(void);
void Speed_Check(void);
#endif
