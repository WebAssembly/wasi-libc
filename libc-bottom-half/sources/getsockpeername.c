#include <errno.h>
#include <netinet/in.h>
#include <wasi/descriptor_table.h>

int getsockname(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
  if (!entry)
    return -1;
  if (!entry->vtable->getsockname) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry->vtable->getsockname(entry->data, addr, addrlen);
}

int getpeername(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
  if (!entry)
    return -1;
  if (!entry->vtable->getpeername) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry->vtable->getpeername(entry->data, addr, addrlen);
}
