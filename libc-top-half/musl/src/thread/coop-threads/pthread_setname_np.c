#define _GNU_SOURCE
#include <string.h>
#include "pthread_impl.h"

int pthread_setname_np(pthread_t thread, const char *name) {
  size_t len = strlen(name);
  if (len > 15)
    return ERANGE;
  strcpy(thread->name, name);
  return 0;
}
