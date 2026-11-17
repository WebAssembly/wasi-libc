#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/libc-find-relpath.h>
#include <wasi/report-error.h>

#if defined(__wasip2__) || defined(__wasip3__)
#include <stddefer.h>
#endif

#include "lock.h"

#ifndef __webp2__
char *__wasilibc_cwd = "/";
// Whether `__wasilibc_cwd` was malloc'd, and so must be freed when `chdir`
// replaces it.
int __wasilibc_cwd_mallocd = 0;
DECLARE_WEAK_LOCK(__wasilibc_cwd_lock);
#endif

#if defined(__wasip2__) || defined(__wasip3__)
static _Atomic _Bool cwd_initialized = false;

// Starts the cwd at `wasi:cli/environment.initial-cwd` the first time the cwd
// is needed. Returns 0 on success, or -1 with `errno` set to `ENOMEM`, in which
// case a later call tries again. The caller must not hold
// `__wasilibc_cwd_lock`.
//
// The initial cwd is resolved against `/`, so `.` and the empty string mean
// `/`. The host's path is used even if no preopen covers it, in which case
// relative paths fail with `ENOENT` instead of silently resolving against `/`.
static int ensure_cwd(void) {
  if (cwd_initialized)
    return 0;

#ifdef __wasip2__
  wasip2_string_t initial;
  bool is_some = environment_initial_cwd(&initial);
#else
  wasip3_string_t initial;
  bool is_some = environment_get_initial_cwd(&initial);
#endif
  char *cwd = NULL;
  if (is_some) {
#ifdef __wasip2__
    defer wasip2_string_free(&initial);
#else
    defer wasip3_string_free(&initial);
#endif
    // Skip leading `/`, `.` and `..` components, since `/..` is `/`, then add
    // back a single `/`.
    const char *rel = (const char *)initial.ptr;
    size_t len = initial.len;
    while (len > 0) {
      size_t skip = 0;
      if (rel[0] == '/' || (rel[0] == '.' && (len == 1 || rel[1] == '/')))
        skip = 1;
      else if (rel[0] == '.' && rel[1] == '.' && (len == 2 || rel[2] == '/'))
        skip = 2;
      if (skip == 0)
        break;
      rel += skip;
      len -= skip;
    }
    if (len > 0) {
      cwd = malloc(len + 2);
      if (cwd == NULL) {
        errno = ENOMEM;
        return -1;
      }
      cwd[0] = '/';
      memcpy(cwd + 1, rel, len);
      cwd[len + 1] = 0;
    }
  }

  WEAK_LOCK(__wasilibc_cwd_lock);
  defer WEAK_UNLOCK(__wasilibc_cwd_lock);
  if (cwd_initialized) {
    // Another thread got here first.
    free(cwd);
    return 0;
  }
  if (cwd != NULL) {
    __wasilibc_cwd = cwd;
    __wasilibc_cwd_mallocd = 1;
  }
  cwd_initialized = true;
  return 0;
}
#endif

char *getcwd(char *buf, size_t size) {
#if defined(__wasip1__) || defined(__wasip2__) || defined(__wasip3__)
#if defined(__wasip2__) || defined(__wasip3__)
  if (ensure_cwd() < 0)
    return NULL;
#endif
  // Critical section contains no yield points, so we can use weak locks.
  WEAK_LOCK(__wasilibc_cwd_lock);
  if (!buf) {
    buf = strdup(__wasilibc_cwd);
    if (!buf) {
      errno = ENOMEM;
      WEAK_UNLOCK(__wasilibc_cwd_lock);
      return NULL;
    }
  } else {
    size_t len = strlen(__wasilibc_cwd);
    if (size < len + 1) {
      errno = ERANGE;
      WEAK_UNLOCK(__wasilibc_cwd_lock);
      return NULL;
    }
    strcpy(buf, __wasilibc_cwd);
  }
  WEAK_UNLOCK(__wasilibc_cwd_lock);
  return buf;
#elif defined(__webp2__)
  (void)buf;
  (void)size;
  WASILIBC_LOG_UNSUPPORTED("getcwd");
  errno = ENOENT;
  return NULL;
#else
#error "Unknown WASI version"
#endif
}

#ifndef __webp2__
// Computes an absolute path for `path` based on the current working directory.
//
// The caller must already hold `__wasilibc_cwd_lock`: this function reads the
// global cwd and writes the static `make_absolute_buf` (a pointer into which is
// returned), so both must stay stable until the caller is done with the result.
static const char *make_absolute(const char *path) {
  static char *make_absolute_buf = NULL;
  static size_t make_absolute_len = 0;

  // If this path is absolute, then we return it as-is.
  if (path[0] == '/') {
    return path;
  }

#ifndef _REENTRANT
  // If the path is empty, or points to the current directory, then return
  // the current directory.
  if (path[0] == 0 || !strcmp(path, ".") || !strcmp(path, "./")) {
    return __wasilibc_cwd;
  }
#endif

  // If the path starts with `./` then we won't be appending that to the cwd.
  if (path[0] == '.' && path[1] == '/')
    path += 2;

  // Otherwise we'll take the current directory, add a `/`, and then add the
  // input `path`. Note that this doesn't do any normalization (like removing
  // `/./`).
  size_t cwd_len = strlen(__wasilibc_cwd);
  size_t path_len = path ? strlen(path) : 0;
  int need_slash = __wasilibc_cwd[cwd_len - 1] == '/' ? 0 : 1;
  size_t alloc_len = cwd_len + path_len + 1 + need_slash;
  if (alloc_len > make_absolute_len) {
    char *tmp = realloc(make_absolute_buf, alloc_len);
    if (tmp == NULL) {
      return NULL;
    }
    make_absolute_buf = tmp;
    make_absolute_len = alloc_len;
  }
  strcpy(make_absolute_buf, __wasilibc_cwd);

#ifdef _REENTRANT
  if (path[0] == 0 || !strcmp(path, ".") || !strcmp(path, "./")) {
    return make_absolute_buf;
  }
#endif

  if (need_slash)
    strcpy(make_absolute_buf + cwd_len, "/");
  strcpy(make_absolute_buf + cwd_len + need_slash, path);
  return make_absolute_buf;
}

// See the documentation in libc-find-relpath.h. This lives here, next to the
// cwd, rather than in `chdir.c` so that all path lookups take the cwd into
// account whether or not `chdir` is linked.
int __wasilibc_find_relpath_alloc(const char *path, const char **abs_prefix,
                                  char **relative_buf, size_t *relative_buf_len,
                                  int can_realloc) {
#if defined(__wasip2__) || defined(__wasip3__)
  if (ensure_cwd() < 0)
    return -1;
#endif
  WEAK_LOCK(__wasilibc_cwd_lock);

  // First, make our path absolute taking the cwd into account.
  const char *abspath = make_absolute(path);
  if (abspath == NULL) {
    WEAK_UNLOCK(__wasilibc_cwd_lock);
    errno = ENOMEM;
    return -1;
  }

  // Next use our absolute path and split it. Find the preopened `fd` parent
  // directory and set `abs_prefix`. Next up we'll be trying to fit `rel`
  // into `relative_buf`.
  const char *rel;
  int fd = __wasilibc_find_abspath(abspath, abs_prefix, &rel);
  if (fd == -1) {
    WEAK_UNLOCK(__wasilibc_cwd_lock);
    return -1;
  }

  size_t rel_len = strlen(rel);
  if (*relative_buf_len < rel_len + 1) {
    if (!can_realloc) {
      WEAK_UNLOCK(__wasilibc_cwd_lock);
      errno = ERANGE;
      return -1;
    }
    char *tmp = realloc(*relative_buf, rel_len + 1);
    if (tmp == NULL) {
      WEAK_UNLOCK(__wasilibc_cwd_lock);
      errno = ENOMEM;
      return -1;
    }
    *relative_buf = tmp;
    *relative_buf_len = rel_len + 1;
  }
  strcpy(*relative_buf, rel);
  WEAK_UNLOCK(__wasilibc_cwd_lock);
  return fd;
}
#endif // !webp2
