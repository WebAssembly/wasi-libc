#include <errno.h>
#include <stddefer.h>
#include <stdint.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

ssize_t recv(int socket, void *restrict buffer, size_t length, int flags) {
#if !defined(__webp2__)
  return recvfrom(socket, buffer, length, flags, NULL, NULL);
#else
  WASILIBC_LOG_UNSUPPORTED("recv");
  errno = ENOSYS;
  return -1;
#endif
}

ssize_t recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
                 struct sockaddr *__restrict addr,
                 socklen_t *__restrict addrlen) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);

  if (buffer == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (entry.vtable->recvfrom == NULL) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->recvfrom(entry.data, buffer, length, flags, addr,
                                addrlen);
#else
  WASILIBC_LOG_UNSUPPORTED("recvfrom");
  errno = ENOSYS;
  return -1;
#endif // !webp2
}
