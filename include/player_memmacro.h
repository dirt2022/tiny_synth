/*
 * SPDX-FileCopyrightText: 2026 dirt2022
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define SAFE_MALLOC_DEF(ptr, size)                                                                                     \
	ptr = malloc(size);                                                                                            \
	if (ptr == NULL) {                                                                                             \
		exit(-1);                                                                                               \
	}

#define SAFE_CALLOC_DEF(ptr, num, elementsize)                                                                         \
	ptr = calloc(num, elementsize);                                                                                \
	if (ptr == NULL) {                                                                                             \
		exit(-1);                                                                                               \
	}

#define SAFE_REALLOC_DEF(ptr, size, tmpptr)                                                                            \
	tmpptr = realloc(ptr, size);                                                                                   \
	if (tmpptr == NULL) {                                                                                          \
		free(ptr);                                                                                             \
		exit(-1);                                                                                               \
	} else {                                                                                                       \
		ptr = tmpptr;                                                                                          \
	}

#define SAFE_MALLOC_CATCH(ptr, size, func, arg)                                                                        \
	ptr = malloc(size);                                                                                            \
	if (ptr == NULL) {                                                                                             \
		func(arg);                                                                                             \
	}

#define SAFE_REALLOC_CATCH(ptr, size, tmpptr, func, arg)                                                               \
	tmpptr = realloc(ptr, size);                                                                                   \
	if (tmpptr == NULL) {                                                                                          \
		free(ptr);                                                                                             \
		func(arg);                                                                                             \
	} else {                                                                                                       \
		ptr = tmpptr;                                                                                          \
	}

#define NFREE_SAFE_REALLOC_CATCH(ptr, size, tmpptr, func, arg)                                                         \
	tmpptr = realloc(ptr, size);                                                                                   \
	if (tmpptr == NULL) {                                                                                          \
		func(arg);                                                                                             \
	} else {                                                                                                       \
		ptr = tmpptr;                                                                                          \
	}

#define SAFE_FREE(ptr)                                                                                                 \
	free(ptr);                                                                                                     \
	ptr = NULL;
