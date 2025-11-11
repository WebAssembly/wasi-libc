#include <errno.h>
#include <wasi/descriptor_table.h>

int shutdown(int socket, int how) {
	descriptor_table_entry_t *entry = descriptor_table_get_ref(socket);
	if (!entry)
		return -1;
        if (entry->vtable->shutdown == NULL) {
                errno = EOPNOTSUPP;
                return -1;
        }
        return entry->vtable->shutdown(entry->data, how);
}
