#include <errno.h>
#include <netinet/in.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int bind(int socket, const struct sockaddr *addr, socklen_t addrlen) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->bind) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->bind(entry.data, addr, addrlen);
#else
  WASILIBC_LOG_UNSUPPORTED("bind");
  errno = ENOSYS;
  return -1;
#endif // !webp2
}
