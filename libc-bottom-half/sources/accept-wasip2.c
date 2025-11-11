#include <errno.h>
#include <netinet/in.h>
#include <wasi/descriptor_table.h>

int accept(int socket, struct sockaddr *restrict addr,
	   socklen_t *restrict addrlen)
{
	return accept4(socket, addr, addrlen, 0);
}

int accept4(int socket, struct sockaddr *restrict addr,
	    socklen_t *restrict addrlen, int flags)
{
	descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
	if (!entry)
		return -1;
        if (!entry->vtable->accept4) {
            errno = EOPNOTSUPP;
            return -1;
        }
        return entry->vtable->accept4(entry->data, addr, addrlen, flags);
}
