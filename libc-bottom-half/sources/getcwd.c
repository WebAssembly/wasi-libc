#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "lock.h"

char *__wasilibc_cwd = "/";

#ifdef _REENTRANT
static __lock_t lock[1];
// Critical section contains no yield points, so we can use weak locks.
void __wasilibc_cwd_lock(void) { WEAK_LOCK(lock); }
void __wasilibc_cwd_unlock(void) { WEAK_UNLOCK(lock); }
#else
#define __wasilibc_cwd_lock() (void)0
#define __wasilibc_cwd_unlock() (void)0
#endif

char *getcwd(char *buf, size_t size) {
  __wasilibc_cwd_lock();
  if (!buf) {
    buf = strdup(__wasilibc_cwd);
    if (!buf) {
      errno = ENOMEM;
      __wasilibc_cwd_unlock();
      return NULL;
    }
  } else {
    size_t len = strlen(__wasilibc_cwd);
    if (size < len + 1) {
      errno = ERANGE;
      __wasilibc_cwd_unlock();
      return NULL;
    }
    strcpy(buf, __wasilibc_cwd);
  }
  __wasilibc_cwd_unlock();
  return buf;
}
