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
 * @file  libpreopen.c
 * Implementation of high-level libpreopen functions.
 *
 * The functions defined in this source file are the highest-level API calls
 * that client code will mostly use (plus po_map_create and po_map_release).
 * po_isprefix is also defined here because it doesn't fit anywhere else.
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef __wasilibc_unmodified_upstream
#else
#include <fcntl.h>
#endif

#include "internal.h"


#ifdef __wasilibc_unmodified_upstream
#else
static
#endif
struct po_map*
po_add(struct po_map *map, const char *path, int fd)
{
	struct po_map_entry *entry;

	po_map_assertvalid(map);

	if (path == NULL || fd < 0) {
		return (NULL);
	}

	if (map->length == map->capacity) {
		map = po_map_enlarge(map);
		if (map == NULL) {
			return (NULL);
		}
	}

	entry = map->entries + map->length;
	map->length++;

	entry->name = strdup(path);
	entry->fd = fd;

#ifdef WITH_CAPSICUM
#ifdef __wasilibc_unmodified_upstream
	if (cap_rights_get(fd, &entry->rights) != 0) {
		return (NULL);
	}
#else
	__wasi_fdstat_t statbuf;
	int r = __wasi_fd_fdstat_get(fd, &statbuf);
	if (r != 0) {
		errno = r;
		return NULL; // TODO: Add an infallible way to get the rights?
	}

	entry->rights_base = statbuf.fs_rights_base;
	entry->rights_inheriting = statbuf.fs_rights_inheriting;
#endif
#endif


	po_map_assertvalid(map);

	return (map);
}

#ifdef __wasilibc_unmodified_upstream
struct po_relpath
po_find(struct po_map* map, const char *path, cap_rights_t *rights)
#else
static struct po_relpath
po_find(struct po_map* map, const char *path,
        __wasi_rights_t rights_base, __wasi_rights_t rights_inheriting)
#endif
{
	const char *relpath ;
	struct po_relpath match = { .relative_path = NULL, .dirfd = -1 };
	size_t bestlen = 0;
	int best = -1;

	po_map_assertvalid(map);

	if (path == NULL) {
		return (match);
	}

	for(size_t i = 0; i < map->length; i++) {
		const struct po_map_entry *entry = map->entries + i;
		const char *name = entry->name;
#ifdef __wasilibc_unmodified_upstream
		size_t len = strnlen(name, MAXPATHLEN);
#else
		size_t len = strlen(name);
#endif

		if ((len <= bestlen) || !po_isprefix(name, len, path)) {
			continue;
		}

#ifdef WITH_CAPSICUM
#ifdef __wasilibc_unmodified_upstream
		if (rights && !cap_rights_contains(&entry->rights, rights)) {
#else
		if ((rights_base & ~entry->rights_base) != 0 ||
                    (rights_inheriting & ~entry->rights_inheriting) != 0) {
#endif
			continue;
		}
#endif

		best = entry->fd;
		bestlen = len;
	}

	relpath = path + bestlen;

	if (*relpath == '/') {
		relpath++;
	}

	if (*relpath == '\0') {
		relpath = ".";
	}

	match.relative_path = relpath;
	match.dirfd = best;

	return match;
}

#ifdef __wasilibc_unmodified_upstream
#else
static
#endif
bool
po_isprefix(const char *dir, size_t dirlen, const char *path)
{
	size_t i;
	assert(dir != NULL);
	assert(path != NULL);
	for (i = 0; i < dirlen; i++)
	{
		if (path[i] != dir[i])
			return false;
	}
	// Ignore trailing slashes in directory names.
	while (i > 0 && dir[i - 1] == '/') {
		--i;
	}
	return path[i] == '/' || path[i] == '\0';
}

#ifdef __wasilibc_unmodified_upstream
int
po_preopen(struct po_map *map, const char *path, int flags, ...)
{
	va_list args;
	int fd, mode;

	va_start(args, flags);
	mode = va_arg(args, int);

	po_map_assertvalid(map);

	if (path == NULL) {
		return (-1);
	}

	fd = openat(AT_FDCWD, path, flags, mode);
	if (fd == -1) {
		return (-1);
	}

	if (po_add(map, path, fd) == NULL) {
		return (-1);
	}

	po_map_assertvalid(map);

	return (fd);
}
#endif

#ifdef __wasilibc_unmodified_upstream
bool
po_print_entry(const char *name, int fd, cap_rights_t rights)
{
	printf(" - name: '%s', fd: %d, rights: <rights>\n",
	       name, fd);
	return (true);
}
#endif
