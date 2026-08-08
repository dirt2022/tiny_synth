/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once
#include <stddef.h>
#include <stdio.h>

struct GlobalStatus {
	float bpm;
	int tracknum;
	float ticks;
	size_t search_offset;
	char SectionInputEnd;
};

struct Input {
	DIH_chaintab_t * line;	
};
void FileParse(FILE * fp,backend_stream_t s);
