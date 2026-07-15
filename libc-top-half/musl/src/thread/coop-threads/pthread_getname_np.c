#define _GNU_SOURCE
#include <string.h>
#include "pthread_impl.h"

int pthread_getname_np(pthread_t thread, char *name, size_t len) {
  size_t n = strlen(thread->name);
  if (n >= len)
    return ERANGE;
  strcpy(name, thread->name);
  return 0;
}
