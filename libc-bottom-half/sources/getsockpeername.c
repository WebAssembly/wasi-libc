#include <errno.h>
#include <netinet/in.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int getsockname(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->getsockname) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry.vtable->getsockname(entry.data, addr, addrlen);
#else
  WEBP2_UNSUPPORTED("getsockname");
#endif // !webp2
}

int getpeername(int socket, struct sockaddr *__restrict addr,
                socklen_t *__restrict addrlen) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->getpeername) {
    errno = ENOTSOCK;
    return -1;
  }
  return entry.vtable->getpeername(entry.data, addr, addrlen);
#else
  WEBP2_UNSUPPORTED("getpeername");
#endif // !webp2
}
