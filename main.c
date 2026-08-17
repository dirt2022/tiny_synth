/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include "include/player_strapi.h"
#include "include/player_limit.h"
#include "include/player_backend.h"
#include "include/player_dataapi.h"
#include "include/player_parser.h"
#include "include/player_math.h"
#include "include/player_thread.h"
#ifdef _OPENMP
#include <omp.h>
#endif

static const char* backendlist[] = {"pa", "file"};

int main(int argc, char* argv[]) {
	// args: ./prog backend_type inputfile
	if (argc != 3) {
		printf("Usage: %s <Backend type> <Input File>\n", argv[0]);
		return -1;
	}

	enum backend_type type;
	type = strlookup(argv[1], (const char**)backendlist, 2);

	if (type == -1) {
		printf("Illegal backend.\n");
		return -1;
	}

	FILE* fp = fopen(argv[2], "rb");
	if (fp == NULL) {
		printf("Unable to open %s\n", argv[2]);
		return -1;
	}

	backend_stream_t stream = backend_init(type);

	sintab_init();
#ifdef _OPENMP
	omp_set_num_threads(omp_get_max_threads());
	thread_num = omp_get_max_threads();
	printf("Running with multi-thread support, available threads %d.\n", omp_get_max_threads());
	printf("Thread speed calibration in progress...\n");
	Init_LoadbalanceTab();
#pragma omp parallel proc_bind(close)
	{
		Speed_Check();
	}
	Calc_Weight();
#endif
	FileParse(fp, stream);
#ifdef _OPENMP
	DeInit_LoadbalanceTab();
#endif
	backend_deinit(type, stream);
	return 0;
}
