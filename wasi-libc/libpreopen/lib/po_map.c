/*-
 * Copyright (c) 2016 Stanley Uche Godfrey
 * Copyright (c) 2016, 2018 Jonathan Anderson
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
 * @file  po_map.c
 * @brief Implementation of po_map management functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

#ifdef __wasilibc_unmodified_upstream__
#else
static
#endif
struct po_map*
po_map_create(int capacity)
{
	struct po_map *map;

	map = malloc(sizeof(struct po_map));
	if (map == NULL) {
		return (NULL);
	}

	map->entries = calloc(sizeof(struct po_map_entry), capacity);
	if (map->entries == NULL) {
		free(map);
		return (NULL);
	}

	map->refcount = 1;
	map->capacity = capacity;
	map->length = 0;

	po_map_assertvalid(map);

	return (map);
}

#ifdef __wasilibc_unmodified_upstream__
#else
static
#endif
struct po_map*
po_map_enlarge(struct po_map *map)
{
	struct po_map_entry *enlarged;
	enlarged = calloc(sizeof(struct po_map_entry), 2 * map->capacity);
	if (enlarged == NULL) {
		return (NULL);
	}
	memcpy(enlarged, map->entries, map->length * sizeof(*enlarged));
	free(map->entries);
	map->entries = enlarged;
	map->capacity = 2 * map->capacity;
	return map;
}

#ifdef __wasilibc_unmodified_upstream__
size_t
po_map_foreach(const struct po_map *map, po_map_iter_cb cb)
{
	struct po_map_entry *entry;
	size_t n;

	po_map_assertvalid(map);

	for (n = 0; n < map->length; n++) {
		entry = map->entries + n;

		if (!cb(entry->name, entry->fd, entry->rights)) {
			break;
		}
	}

	return (n);
}
#endif

#ifdef __wasilibc_unmodified_upstream__
#else
static
#endif
void
po_map_release(struct po_map *map)
{
	if (map == NULL) {
		return;
	}

	po_map_assertvalid(map);

	map->refcount -= 1;

	if (map->refcount == 0) {
		free(map->entries);
		free(map);
	}
}
