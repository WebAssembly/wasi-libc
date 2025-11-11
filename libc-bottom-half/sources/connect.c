#include <errno.h>
#include <netinet/in.h>
#include <wasi/descriptor_table.h>

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
	descriptor_table_entry_t *entry = descriptor_table_get_ref(fd);
	if (!entry)
		return -1;
        if (!entry->vtable->connect) {
		errno = EOPNOTSUPP;
		return -1;
        }
        return entry->vtable->connect(entry->data, addr, addrlen);
}
