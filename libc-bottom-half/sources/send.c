#include <errno.h>
#include <stddefer.h>
#include <stdint.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

ssize_t send(int socket, const void *buffer, size_t length, int flags) {
#if !defined(__webp2__)
  return sendto(socket, buffer, length, flags, NULL, 0);
#else
  WASILIBC_LOG_UNSUPPORTED("send");
  errno = ENOTSOCK;
  return -1;
#endif
}

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
               const struct sockaddr *addr, socklen_t addrlen) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);

  if (buffer == NULL) {
    errno = EINVAL;
    return -1;
  }

  if (entry.vtable->sendto == NULL) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->sendto(entry.data, buffer, length, flags, addr, addrlen);
#else
  WASILIBC_LOG_UNSUPPORTED("sendto");
  errno = ENOTSOCK;
  return -1;
#endif // !webp2
}
