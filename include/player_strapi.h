/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

int strlookup(const char* str, const char** tab, size_t lines_of_tab);
float* str2farray(const char* str, size_t* wrote_array_len);
unsigned int next_letter_offset(const char* str, int* is_space_detected);
unsigned int next_space_offset(const char* str);
size_t numlen(char* str);
int myfgets(char* buffer, int size, FILE* fp);
