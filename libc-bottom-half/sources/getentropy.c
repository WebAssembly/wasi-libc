#include <errno.h>
#include <unistd.h>
#include <wasi/api.h>
#include <wasi/libc.h>

int __getentropy(void *buffer, size_t len) {
  if (len > 256) {
    errno = EIO;
    return -1;
  }

#ifdef __wasip2__
  int r = __wasilibc_random(buffer, len);
#else
  int r = __wasi_random_get(buffer, len);
#endif
  if (r != 0) {
    errno = r;
    return -1;
  }
  return 0;
}
weak_alias(__getentropy, getentropy);
