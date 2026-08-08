/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

enum backend_type {
	PulseAudio=0,
	File=1
};

typedef void * backend_stream_t;

backend_stream_t backend_init(enum backend_type type);
void backend_deinit(enum backend_type type,backend_stream_t s);

extern int(* BackendWrite)(backend_stream_t s,void * buffer,size_t size);
