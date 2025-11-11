#include <errno.h>
#include <netinet/in.h>
#include <wasi/descriptor_table.h>

int listen(int socket, int backlog)
{
	descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
	if (!entry)
		return -1;

	if (backlog < 0) {
		// POSIX:
		// > If listen() is called with a backlog argument value that is
		// > less than 0, the function behaves as if it had been called
		// > with a backlog argument value of 0.
		backlog = 0;
	}
        if (!entry->vtable->listen) {
		errno = EOPNOTSUPP;
		return -1;
        }
        return entry->vtable->listen(entry->data, backlog);
}
