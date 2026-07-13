#include <errno.h>
#include <stddefer.h>
#include <stdint.h>
#include <wasi/descriptor_table.h>

ssize_t recv(int socket, void *restrict buffer, size_t length, int flags) {
  return recvfrom(socket, buffer, length, flags, NULL, NULL);
}

ssize_t recvfrom(int socket, void *__restrict buffer, size_t length, int flags,
                 struct sockaddr *__restrict addr,
                 socklen_t *__restrict addrlen) {
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
}
