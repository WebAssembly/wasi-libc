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
 * @file   po_libc_wrappers.c
 * @brief  Wrappers of libc functions that access global variables.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <fcntl.h>
#ifdef __wasilibc_unmodified_upstream // dlfcn
#include <dlfcn.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef __wasilibc_unmodified_upstream
#else
#include <errno.h>
#include <wasi/libc.h>
#endif

#include "internal.h"
#ifdef __wasilibc_unmodified_upstream
#else
// Make all of the po_* implementation details private.
#include "libpreopen.c"
#include "po_map.c"
#include "po_err.c"
#endif

/**
 * A default po_map that can be used implicitly by libc wrappers.
 *
 * @internal
 */
static struct po_map *global_map;

/**
 * Find a relative path within the po_map given by SHARED_MEMORYFD (if it
 * exists).
 *
 * @returns  a struct po_relpath with dirfd and relative_path as set by po_find
 *           if there is an available po_map, or AT_FDCWD/path otherwise
 */
#ifdef __wasilibc_unmodified_upstream
static struct po_relpath find_relative(const char *path, cap_rights_t *);
#else
static struct po_relpath find_relative(const char *path,
                                       __wasi_rights_t rights_base,
                                       __wasi_rights_t rights_inheriting);
#endif

/**
 * Get the map that was handed into the process via `SHARED_MEMORYFD`
 * (if it exists).
 */
static struct po_map*	get_shared_map(void);


/*
 * Wrappers around system calls:
 */

/**
 * Capability-safe wrapper around the `_open(2)` system call.
 *
 * `_open(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `openat(2)` if
 * possible. If the current po_map does not contain the sought-after path,
 * this wrapper will call `openat(AT_FDCWD, original_path, ...)`, which is
 * the same as the unwrapped `open(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
#ifdef __wasilibc_unmodified_upstream
_open(const char *path, int flags, ...)
#else
/* We just need a plain open definition. */
open(const char *path, int flags, ...)
#endif
{
	struct po_relpath rel;
	va_list args;
	int mode;

#ifdef __wasilibc_unmodified_upstream
	va_start(args, flags);
	mode = va_arg(args, int);
	rel = find_relative(path, NULL);
#else
	if (flags & O_CREAT) {
		va_start(args, flags);
		mode = va_arg(args, int);
		va_end(args);
	} else {
		mode = 0;
	}
	rel = find_relative(path, __WASI_RIGHT_PATH_OPEN, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel.dirfd == -1) {
		errno = ENOTCAPABLE;
		return -1;
	}
#endif

#ifdef __wasilibc_unmodified_upstream
	// If the file is already opened, no need of relative opening!
	if( strcmp(rel.relative_path,".") == 0 )
		return dup(rel.dirfd);
	else
		return openat(rel.dirfd, rel.relative_path, flags, mode);
#else
	return openat(rel.dirfd, rel.relative_path, flags, mode);
#endif
}

/**
 * Capability-safe wrapper around the `access(2)` system call.
 *
 * `access(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `faccessat(2)` if
 * possible. If the current po_map does not contain the sought-after path,
 * this wrapper will call `faccessat(AT_FDCWD, original_path, ...)`, which is
 * the same as the unwrapped `access(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
access(const char *path, int mode)
{
#ifdef __wasilibc_unmodified_upstream
	struct po_relpath rel = find_relative(path, NULL);
#else
	struct po_relpath rel = find_relative(path, __WASI_RIGHT_PATH_FILESTAT_GET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel.dirfd == -1) {
		errno = ENOTCAPABLE;
		return -1;
	}
#endif

	return faccessat(rel.dirfd, rel.relative_path, mode,0);
}

#ifdef __wasilibc_unmodified_upstream
/**
 * Capability-safe wrapper around the `connect(2)` system call.
 *
 * `connect(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `connectat(2)` if
 * possible. If the current po_map does not contain the sought-after path, this
 * wrapper will call `connectat(AT_FDCWD, original_path, ...)`, which is the
 * same as the unwrapped `connect(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
connect(int s, const struct sockaddr *name, socklen_t namelen)
{
	struct po_relpath rel;

	if (name->sa_family == AF_UNIX) {
	    struct sockaddr_un *usock = (struct sockaddr_un *)name;
#ifdef __wasilibc_unmodified_upstream
	    rel = find_relative(usock->sun_path, NULL);
	    strlcpy(usock->sun_path, rel.relative_path, sizeof(usock->sun_path));
#else
	    rel = find_relative(usock->sun_path, __WASI_RIGHT_CONNECT, 0);
	    if (strlen(rel.relative_path) + 1 > sizeof(usock->sun_path)) {
		errno = ENOMEM;
		return -1;
	    }

	    // If we can't find a preopened directory handle to open this file with,
	    // indicate that the program lacks the capabilities.
	    if (rel.dirfd == -1) {
		errno = ENOTCAPABLE;
		return -1;
	    }

	    strcpy(usock->sun_path, rel.relative_path);
#endif
	    return connectat(rel.dirfd, s, name, namelen);
	}

	return connectat(AT_FDCWD, s, name, namelen);
}
#endif

/**
 * Capability-safe wrapper around the `eaccess(2)` system call.
 *
 * `eaccess(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `faccessat(2)` if
 * possible. If the current po_map does not contain the sought-after path, this
 * wrapper will call `faccessat(AT_FDCWD, original_path, ...)`, which is the
 * same as the unwrapped `eaccess(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
eaccess(const char *path, int mode)
{
#ifdef __wasilibc_unmodified_upstream
	struct po_relpath rel = find_relative(path, NULL);
#else
	struct po_relpath rel = find_relative(path, __WASI_RIGHT_PATH_FILESTAT_GET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}
#endif

	return faccessat(rel.dirfd, rel.relative_path, mode, 0);
}

/**
 * Capability-safe wrapper around the `lstat(2)` system call.
 *
 * `lstat(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `fstatat(2)` if
 * possible. If the current po_map does not contain the sought-after path,
 * this wrapper will call `fstatat(AT_FDCWD, original_path, ...)`, which is
 * the same as the unwrapped `lstat(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
lstat(const char *path, struct stat *st)
{
#ifdef __wasilibc_unmodified_upstream
	struct po_relpath rel = find_relative(path, NULL);
#else
	struct po_relpath rel = find_relative(path, __WASI_RIGHT_PATH_FILESTAT_GET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}
#endif

	return fstatat(rel.dirfd, rel.relative_path,st,AT_SYMLINK_NOFOLLOW);
}

#ifdef __wasilibc_unmodified_upstream
/**
 * Capability-safe wrapper around the `open(2)` system call.
 *
 * `open(2)` will behave just like `_open(2)` if the varargs are unpacked and
 *  passed.
 */
int
open(const char *path, int flags, ...)
{
	va_list args;
	int mode;

	va_start(args, flags);
	mode = va_arg(args, int);
	return _open(path, flags, mode);
}
#endif

/**
 * Capability-safe wrapper around the `rename(2)` system call.
 *
 * `rename(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `renameat(2)` if
 * possible. If the current po_map does not contain the sought-after path,
 * this wrapper will call `renameat(AT_FDCWD, original_path, ...)`, which is
 * the same as the unwrapped `rename(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
rename(const char *from, const char *to)
{
#ifdef __wasilibc_unmodified_upstream
	struct po_relpath rel_from = find_relative(from, NULL);
	struct po_relpath rel_to = find_relative(to, NULL);
#else
	struct po_relpath rel_from = find_relative(from, __WASI_RIGHT_PATH_RENAME_SOURCE, 0);
	struct po_relpath rel_to = find_relative(to, __WASI_RIGHT_PATH_RENAME_TARGET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_from.dirfd == -1 || rel_to.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}
#endif

	return renameat(rel_from.dirfd, rel_from.relative_path, rel_to.dirfd,
		rel_to.relative_path);
}

/**
 * Capability-safe wrapper around the `stat(2)` system call.
 *
 * `stat(2)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `fstatat(2)` if
 * possible. If the current po_map does not contain the sought-after path,
 * this wrapper will call `fstatat(AT_FDCWD, original_path, ...)`, which is
 * the same as the unwrapped `stat(2)` call (i.e., will fail with `ECAPMODE`).
 */
int
stat(const char *path, struct stat *st)
{
#ifdef __wasilibc_unmodified_upstream
	struct po_relpath rel = find_relative(path, NULL);
#else
	struct po_relpath rel = find_relative(path, __WASI_RIGHT_PATH_FILESTAT_GET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}
#endif

	return fstatat(rel.dirfd, rel.relative_path,st, AT_SYMLINK_NOFOLLOW);
}

/*
 * Wrappers around other libc calls:
 */

#ifdef __wasilibc_unmodified_upstream
/**
 * Capability-safe wrapper around the `dlopen(3)` libc function.
 *
 * `dlopen(3)` accepts a path argument that can reference the global filesystem
 * namespace. This is not a capability-safe operation, so this wrapper function
 * attempts to look up the path (or a prefix of it) within the current global
 * po_map and converts the call into the capability-safe `fdlopen(3)` if
 * possible. If the current po_map does not contain the sought-after path, this
 * wrapper will call `fdlopen(openat(AT_FDCWD, original_path), ...)`, which is
 * the same as the unwrapped `dlopen(3)` call (i.e., will fail with `ECAPMODE`).
 */
void *
dlopen(const char *path, int mode)
{
	struct po_relpath rel = find_relative(path, NULL);

	return fdlopen(openat(rel.dirfd, rel.relative_path, 0, mode), mode);
}
#endif
#ifdef __wasilibc_unmodified_upstream
#else
#include <dirent.h>

int
unlink(const char *pathname)
{
	struct po_relpath rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_UNLINK_FILE, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_pathname.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return __wasilibc_rmfileat(rel_pathname.dirfd, rel_pathname.relative_path);
}

int
rmdir(const char *pathname)
{
	struct po_relpath rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_REMOVE_DIRECTORY, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_pathname.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return __wasilibc_rmdirat(rel_pathname.dirfd, rel_pathname.relative_path);
}

int
remove(const char *pathname)
{
	struct po_relpath rel_pathname = find_relative(pathname,
	                                               __WASI_RIGHT_PATH_UNLINK_FILE |
	                                               __WASI_RIGHT_PATH_REMOVE_DIRECTORY,
                                                       0);

	// If searching for both file and directory rights failed, try searching
	// for either individually.
	if (rel_pathname.dirfd == -1) {
		rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_UNLINK_FILE, 0);
		if (rel_pathname.dirfd == -1) {
			rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_REMOVE_DIRECTORY, 0);
		}
	}

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_pathname.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	int r = __wasilibc_rmfileat(rel_pathname.dirfd, rel_pathname.relative_path);
	if (r != 0 && (errno == EISDIR || errno == ENOTCAPABLE))
		r = __wasilibc_rmdirat(rel_pathname.dirfd, rel_pathname.relative_path);
	return r;
}

int
link(const char *oldpath, const char *newpath)
{
	struct po_relpath rel_oldpath = find_relative(oldpath, __WASI_RIGHT_PATH_LINK_SOURCE, 0);
	struct po_relpath rel_newpath = find_relative(newpath, __WASI_RIGHT_PATH_LINK_TARGET, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_oldpath.dirfd == -1 || rel_newpath.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return linkat(rel_oldpath.dirfd, rel_oldpath.relative_path,
	              rel_newpath.dirfd, rel_newpath.relative_path,
	              0);
}

int
mkdir(const char *pathname, mode_t mode)
{
	struct po_relpath rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_CREATE_DIRECTORY, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_pathname.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return mkdirat(rel_pathname.dirfd, rel_pathname.relative_path, mode);
}

DIR *
opendir(const char *name)
{
	struct po_relpath rel_name = find_relative(name, __WASI_RIGHT_PATH_OPEN, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_name.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return NULL;
	}

	return opendirat(rel_name.dirfd, rel_name.relative_path);
}

ssize_t
readlink(const char *pathname, char *buf, size_t bufsiz)
{
	struct po_relpath rel_pathname = find_relative(pathname, __WASI_RIGHT_PATH_READLINK, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_pathname.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return readlinkat(rel_pathname.dirfd, rel_pathname.relative_path,
	                  buf, bufsiz);
}

int
scandir(const char *dirp, struct dirent ***namelist,
	int (*filter)(const struct dirent *),
	int (*compar)(const struct dirent **, const struct dirent **))
{
	struct po_relpath rel_dirp = find_relative(dirp,
	                                           __WASI_RIGHT_PATH_OPEN,
	                                           __WASI_RIGHT_FD_READDIR);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_dirp.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return scandirat(rel_dirp.dirfd, rel_dirp.relative_path,
	                 namelist, filter, compar);
}

int
symlink(const char *target, const char *linkpath)
{
	struct po_relpath rel_linkpath = find_relative(linkpath, __WASI_RIGHT_PATH_SYMLINK, 0);

	// If we can't find a preopened directory handle to open this file with,
	// indicate that the program lacks the capabilities.
	if (rel_linkpath.dirfd == -1) {
	    errno = ENOTCAPABLE;
	    return -1;
	}

	return symlinkat(target, rel_linkpath.dirfd, rel_linkpath.relative_path);
}
#endif

#ifdef __wasilibc_unmodified_upstream
#else
static
#endif
/* Provide tests with mechanism to set our static po_map */
void
po_set_libc_map(struct po_map *map)
{
	po_map_assertvalid(map);

	map->refcount += 1;

	if (global_map != NULL) {
		po_map_release(global_map);
	}

	global_map = map;
}

static struct po_relpath
#ifdef __wasilibc_unmodified_upstream
find_relative(const char *path, cap_rights_t *rights)
#else
find_relative(const char *path,
              __wasi_rights_t rights_base,
              __wasi_rights_t rights_inheriting)
#endif
{
	struct po_relpath rel;
	struct po_map *map;

	map = get_shared_map();
#ifdef __wasilibc_unmodified_upstream
	if (map == NULL) {
		rel.dirfd = AT_FDCWD;
		rel.relative_path = path;
	} else {
		rel = po_find(map, path, NULL);
	}
#else
	rel = po_find(map, path, rights_base, rights_inheriting);
#endif

	return (rel);
}

static struct po_map*
get_shared_map()
{
#ifdef __wasilibc_unmodified_upstream
	struct po_map *map;
	char *end, *env;
	long fd;

	// Do we already have a default map?
	if (global_map) {
		po_map_assertvalid(global_map);
		return (global_map);
	}

	// Attempt to unwrap po_map from a shared memory segment specified by
	// SHARED_MEMORYFD
	env = getenv("SHARED_MEMORYFD");
	if (env == NULL || *env == '\0') {
		return (NULL);
	}

	// We expect this environment variable to be an integer and nothing but
	// an integer.
	fd = strtol(env, &end, 10);
	if (*end != '\0') {
		return (NULL);
	}

	map = po_unpack(fd);
	if (map == NULL) {
		return (NULL);
	}

	global_map = map;

	return (map);
#else
	assert(global_map);
	po_map_assertvalid(global_map);
	return global_map;
#endif
}
#ifdef __wasilibc_unmodified_upstream
#else
void
__wasilibc_init_preopen(void)
{
	global_map = po_map_create(4);
	if (global_map == NULL)
		__builtin_trap();
	po_map_assertvalid(global_map);
}

/*
 * Register the given pre-opened file descriptor under the given path.
 */
int
__wasilibc_register_preopened_fd(int fd, const char *path)
{
	po_map_assertvalid(global_map);

	if (path == NULL) {
		return -1;
	}

#ifdef _REENTRANT
#error "__wasilibc_register_preopened_fd doesn't yet support multiple threads"
#endif

	struct po_map *map = po_add(global_map, path, fd);
	if (map == NULL) {
		return -1;
	}

	po_map_assertvalid(map);
	global_map = map;

	return 0;
}

int __wasilibc_find_relpath(const char *path,
                            __wasi_rights_t rights_base,
                            __wasi_rights_t rights_inheriting,
                            const char **relpath)
{
    struct po_relpath rel = find_relative(path, rights_base, rights_inheriting);
    *relpath = rel.relative_path;
    return rel.dirfd;
}
#endif
