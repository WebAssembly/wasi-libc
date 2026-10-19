#include <errno.h>
#include <netinet/in.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

int getsockopt(int sockfd, int level, int optname, void *restrict optval,
               socklen_t *restrict optlen) {
  descriptor_table_entry_t entry;
  if (descriptor_table_get(sockfd, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);

  if (optval == NULL || optlen == NULL || *optlen < sizeof(int)) {
    // FYI, the protocol-specific implementations implicitly depend on these
    // checks.
    errno = EINVAL;
    return -1;
  }

  if (!entry.vtable->getsockopt) {
    errno = ENOPROTOOPT;
    return -1;
  }

  return entry.vtable->getsockopt(entry.data, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval,
               socklen_t optlen) {
  descriptor_table_entry_t entry;
  if (descriptor_table_get(sockfd, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);

  if (optval == NULL || optlen < sizeof(int)) {
    // FYI, the protocol-specific implementations implicitly depend on these
    // checks.
    errno = EINVAL;
    return -1;
  }

  if (!entry.vtable->setsockopt) {
    errno = ENOPROTOOPT;
    return -1;
  }

  return entry.vtable->setsockopt(entry.data, level, optname, optval, optlen);
}
