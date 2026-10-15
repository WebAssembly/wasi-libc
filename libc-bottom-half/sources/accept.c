#include <errno.h>
#include <netinet/in.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

#if defined(__webp2__)
#include <wasi/report-error.h>
#endif

int accept(int socket, struct sockaddr *restrict addr,
           socklen_t *restrict addrlen) {
#if !defined(__webp2__)
  return accept4(socket, addr, addrlen, 0);
#else
  WEBP2_UNSUPPORTED("accept");
#endif
}

int accept4(int socket, struct sockaddr *restrict addr,
            socklen_t *restrict addrlen, int flags) {
#if !defined(__webp2__)
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (!entry.vtable->accept4) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->accept4(entry.data, addr, addrlen, flags);
#else
  WEBP2_UNSUPPORTED("accept4");
#endif // !webp2
}
