#include <errno.h>
#include <wasi/version.h>

#ifdef __wasip3__

int __wasilibc_init_stdio() {
  // TODO(wasip3)
  errno = EOPNOTSUPP;
  return -1;
}
#endif // __wasip3__
