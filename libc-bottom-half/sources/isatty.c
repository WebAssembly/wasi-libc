#ifdef __wasilibc_use_wasip2
#include <common/errors.h>
#include <wasi/file_utils.h>
#include <wasi/wasip2.h>
#else
#include <wasi/api.h>
#endif
#include <__errno.h>
#include <__function___isatty.h>

int __isatty(int fd) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor into an internal handle
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
  if (!entry)
    return 0;
  if (!entry->vtable->isatty) {
    errno = ENOTTY;
    return 0;
  }

  return entry->vtable->isatty(entry->data);
#else

  __wasi_fdstat_t statbuf;
  int r = __wasi_fd_fdstat_get(fd, &statbuf);
  if (r != 0) {
    errno = r;
    return 0;
  }

  // A tty is a character device that we can't seek or tell on.
  if (statbuf.fs_filetype != __WASI_FILETYPE_CHARACTER_DEVICE ||
      (statbuf.fs_rights_base &
       (__WASI_RIGHTS_FD_SEEK | __WASI_RIGHTS_FD_TELL)) != 0) {
    errno = __WASI_ERRNO_NOTTY;
    return 0;
  }

  return 1;
#endif
}
#ifdef __wasilibc_use_wasip2
weak_alias(__isatty, isatty);
#else
extern __typeof(__isatty) isatty __attribute__((weak, alias("__isatty")));
#endif
