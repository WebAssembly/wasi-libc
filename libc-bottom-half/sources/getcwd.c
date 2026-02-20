#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "lock.h"

char *__wasilibc_cwd = "/";

#ifdef _REENTRANT
static __lock_t lock[1];
#endif

char *getcwd(char *buf, size_t size) {
  // Critical section contains no yield points, so we can use weak locks.
  WEAK_LOCK(lock);
  if (!buf) {
    buf = strdup(__wasilibc_cwd);
    if (!buf) {
      errno = ENOMEM;
      WEAK_UNLOCK(lock);
      return NULL;
    }
  } else {
    size_t len = strlen(__wasilibc_cwd);
    if (size < len + 1) {
      errno = ERANGE;
      WEAK_UNLOCK(lock);
      return NULL;
    }
    strcpy(buf, __wasilibc_cwd);
  }
  WEAK_UNLOCK(lock);
  return buf;
}
