#include <errno.h>
#include <stddefer.h>
#include <wasi/descriptor_table.h>

int shutdown(int socket, int how) {
  descriptor_table_entry_t entry;
  if (descriptor_table_get(socket, &entry) < 0)
    return -1;
  defer descriptor_table_entry_dec(entry);
  if (entry.vtable->shutdown == NULL) {
    errno = EOPNOTSUPP;
    return -1;
  }
  return entry.vtable->shutdown(entry.data, how);
}
