#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/cwd.h>
#include <wasi/report-error.h>

#if defined(__wasip2__) || defined(__wasip3__)
#include <stddefer.h>
#endif

#include "lock.h"

#ifndef __webp2__
char *__wasilibc_cwd = "/";
DECLARE_WEAK_LOCK(__wasilibc_cwd_lock);
#endif

#if defined(__wasip2__) || defined(__wasip3__)
int __wasilibc_cwd_mallocd = 0;
static _Atomic _Bool cwd_initialized = false;

// The initial cwd is resolved against `/`, so `.` and the empty string mean
// `/`. The host's path is used even if no preopen covers it, in which case
// relative paths fail with `ENOENT` instead of silently resolving against `/`.
int __wasilibc_ensure_cwd(void) {
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

void __wasilibc_reset_cwd(void) {
  WEAK_LOCK(__wasilibc_cwd_lock);
  defer WEAK_UNLOCK(__wasilibc_cwd_lock);
  if (__wasilibc_cwd_mallocd)
    free(__wasilibc_cwd);
  __wasilibc_cwd = "/";
  __wasilibc_cwd_mallocd = 0;
  cwd_initialized = false;
}
#endif

char *getcwd(char *buf, size_t size) {
#if defined(__wasip1__) || defined(__wasip2__) || defined(__wasip3__)
#if defined(__wasip2__) || defined(__wasip3__)
  if (__wasilibc_ensure_cwd() < 0)
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
