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

static char* convert_to_posix(char *path) {
  size_t path_len = strlen(path);
  if (path_len == 0) {
    return (NULL);
  }

  // does it have Windows-style "C:\" or "C:/" prefix?
  int offset = 0;
  if (path_len >= 3 && path[1] == ':' && (path[2] == '\\' || path[2] == '/')) {
    offset = 2;
  }

  // allocate
  size_t po_path_len = path_len - offset + 1;
  char *po_path = calloc(sizeof(char), po_path_len);

  // convert to POSIX "/"
  int i = offset;
  char c;
  while (i < path_len) {
    c = path[i];
    if (c == '\\') {
      po_path[i - offset] = '/';
    } else {
      po_path[i - offset] = c;
    }
    ++i;
  }

  po_path[po_path_len] = '\0';

  return po_path;
}

#ifdef __wasilibc_unmodified_upstream
#else
static
#endif
struct po_map*
po_add(struct po_map *map, const char *path, int fd)
{
	struct po_map_entry *entry;
  char *po_path;

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

#ifdef __wasilibc_unmodified_upstream
	entry->name = path;
#else
	entry->name = convert_to_posix(path);
#endif
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
po_find(struct po_map* map, const char *po_path,
        __wasi_rights_t rights_base, __wasi_rights_t rights_inheriting)
#endif
{
#ifdef __wasilibc_unmodified_upstream
#else
  char *path = convert_to_posix(po_path);
#endif

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
		bool any_matches = false;

		if (path[0] != '/' && (path[0] != '.' || (path[1] != '/' && path[1] != '\0'))) {
			// We're matching a relative path that doesn't start with "./" and isn't ".".
			if (len >= 2 && name[0] == '.' && name[1] == '/') {
				// The entry starts with "./", so skip that prefix.
				name += 2;
				len -= 2;
			} else if (len == 1 && name[0] == '.') {
				// The entry is ".", so match it as an empty string.
				name += 1;
				len -= 1;
			}
		}
#endif

#ifdef __wasilibc_unmodified_upstream
		if ((len <= bestlen) || !po_isprefix(name, len, path)) {
#else
		if ((any_matches && len <= bestlen) || !po_isprefix(name, len, path)) {
#endif
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
#ifdef __wasilibc_unmodified_upstream
#else
		any_matches = true;
#endif
	}

	relpath = path + bestlen;

	while (*relpath == '/') {
		relpath++;
	}

	if (*relpath == '\0') {
		relpath = ".";
	}

	match.relative_path = relpath;
	match.dirfd = best;

#ifdef __wasilibc_unmodified_upstream
#else
  free(path);
#endif

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
#ifdef __wasilibc_unmodified_upstream
#else
	// Allow an empty string as a prefix of any relative path.
	if (path[0] != '/' && dirlen == 0)
		return true;
#endif
	for (i = 0; i < dirlen; i++)
	{
		if (path[i] != dir[i])
			return false;
	}
#ifdef __wasilibc_unmodified_upstream
#else
	// Ignore trailing slashes in directory names.
	while (i > 0 && dir[i - 1] == '/') {
		--i;
	}
#endif
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
