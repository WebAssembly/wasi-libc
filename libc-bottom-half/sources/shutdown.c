#include <errno.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int shutdown(int socket, int how) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (entry.vtable->shutdown == NULL) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->shutdown(entry.data, how);
#else
  WASILIBC_LOG_UNSUPPORTED("shutdown");
  errno = ENOSYS;
  return -1;
#endif // !webp2
}
