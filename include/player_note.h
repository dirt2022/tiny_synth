/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once
int parse_note(char* buffer, struct WaveArgs* arg, const struct GlobalStatus* gs, const struct LoudnessLoopTab* llpt,
	       float ticks, size_t len);
