#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "lock.h"

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

#ifndef __webp2__
char *__wasilibc_cwd = "/";
DECLARE_WEAK_LOCK(__wasilibc_cwd_lock);
#endif

char *getcwd(char *buf, size_t size) {
#if !defined(__webp2__)
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
#else
  WASILIBC_LOG_UNSUPPORTED("getcwd");
  errno = ENOENT;
  return NULL;
#endif // !webp2
}
