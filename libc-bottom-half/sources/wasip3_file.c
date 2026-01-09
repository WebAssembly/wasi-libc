#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__

int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag) {
  // TODO(wasip3)
  errno = EOPNOTSUPP;
  return -1;
}

#endif // __wasip3__
