#include <errno.h>
#include <stdint.h>
#include <wasi/descriptor_table.h>

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	return sendto(socket, buffer, length, flags, NULL, 0);
}

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
	       const struct sockaddr *addr, socklen_t addrlen)
{
	descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
	if (!entry)
		return -1;

	if (buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

        if (entry->vtable->sendto == NULL) {
                errno = EOPNOTSUPP;
                return -1;
        }
        return entry->vtable->sendto(entry->data, buffer, length, flags,
                                     addr, addrlen);
}
