#include <errno.h>
#include <netinet/in.h>
#include <wasi/sockets_utils.h>

int bind(int socket, const struct sockaddr *addr, socklen_t addrlen)
{
	descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
	if (!entry)
		return -1;
        if (!entry->vtable->bind) {
            errno = EOPNOTSUPP;
            return -1;
        }
        return entry->vtable->bind(entry->data, addr, addrlen);
}
