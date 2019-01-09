/*-
 * Copyright (c) 2016 Stanley Uche Godfrey
 * Copyright (c) 2018 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed at Memorial University under the
 * NSERC Discovery program (RGPIN-2015-06048).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * @file  po_err.c
 * @brief Error handling for libpreopen
 */

#ifdef __cowslibc_unmodified_upstream__
#include <sys/cdefs.h>
#include <sys/param.h>
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __cowslibc_unmodified_upstream__
#include <sys/wait.h>
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"
#include "libpreopen.h"

#ifdef __cowslibc_unmodified_upstream__
/* Disable all this error reporting code. */
static char error_buffer[1024];
#endif

#ifdef __cowslibc_unmodified_upstream__
#else
static
#endif
#if !defined(NDEBUG)
void
po_map_assertvalid(const struct po_map *map)
{
	const struct po_map_entry *entry;
	size_t i;

	assert(map->refcount > 0);
	assert(map->length <= map->capacity);
	assert(map->entries != NULL || map->capacity == 0);

	for (i = 0; i < map->length; i++) {
		entry = map->entries + i;

		assert(entry->name != NULL);
		assert(entry->fd >= 0);
	}
}
#endif /* !defined(NDEBUG) */

#ifdef __cowslibc_unmodified_upstream__
void
po_errormessage(const char *msg)
{

	snprintf(error_buffer, sizeof(error_buffer), "%s: error %d",
		msg, errno);
}

const char*
po_last_error()
{

	return (error_buffer);
}
#endif
