#include <errno.h>
#include <netinet/in.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

int getsockname(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->getsockname) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry.vtable->getsockname(entry.data, addr, addrlen);
}

int getpeername(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->getpeername) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry.vtable->getpeername(entry.data, addr, addrlen);
}
