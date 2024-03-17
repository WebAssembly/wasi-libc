/*
 * This file provides a global hashtable for tracking `wasi-libc`-managed file
 * descriptors.
 *
 * WASI Preview 2 has no notion of file descriptors and instead uses unforgeable
 * resource handles (which are currently represented as integers at the ABI
 * level, used as indices into per-component tables managed by the host).
 * Moreover, there's not necessarily a one-to-one correspondence between POSIX
 * file descriptors and resource handles (e.g. a TCP connection may require
 * separate handles for reading, writing, and polling the same connection).  We
 * use this table to map each POSIX descriptor to a set of one or more handles.
 *
 * As of this writing, we still rely on the WASI Preview 1 adapter
 * (https://github.com/bytecodealliance/wasmtime/tree/main/crates/wasi-preview1-component-adapter)
 * to manage non-socket descriptors, so currently this table only tracks TCP and
 * UDP sockets.  We use the adapter's `adapter_open_badfd` and
 * `adapter_close_badfd` functions to reserve and later close descriptors to
 * avoid confusion (e.g. if an application tries to use Preview 1 host functions
 * directly for socket operations rather than go through `wasi-libc`).
 * Eventually, we'll switch `wasi-libc` over to Preview 2 entirely, at which
 * point we'll no longer need the adapter.  At that point, all file descriptors
 * will be managed exclusively in this table.
 */

#include <wasi/descriptor_table.h>

__attribute__((__import_module__("wasi_snapshot_preview1"),
	       __import_name__("adapter_open_badfd"))) extern int32_t
	__wasi_preview1_adapter_open_badfd(int32_t);

static bool wasi_preview1_adapter_open_badfd(int *fd)
{
	return __wasi_preview1_adapter_open_badfd((int32_t)fd) == 0;
}

__attribute__((__import_module__("wasi_snapshot_preview1"),
	       __import_name__("adapter_close_badfd"))) extern int32_t
	__wasi_preview1_adapter_close_badfd(int32_t);

static bool wasi_preview1_adapter_close_badfd(int fd)
{
	return __wasi_preview1_adapter_close_badfd(fd) == 0;
}

/*
 * This hash table is based on the one in musl/src/search/hsearch.c, but uses
 * integer keys and supports a `remove` operation.  Note that I've switched from
 * quadratic to linear probing in order to make `remove` simple and efficient,
 * with the tradeoff that clustering is more likely.  See also
 * https://en.wikipedia.org/wiki/Open_addressing.
 */

#define MINSIZE 8
#define MAXSIZE ((size_t)-1 / 2 + 1)

typedef struct {
	bool occupied;
	int key;
	descriptor_table_entry_t entry;
} descriptor_table_item_t;

typedef struct {
	descriptor_table_item_t *entries;
	size_t mask;
	size_t used;
} descriptor_table_t;

static descriptor_table_t global_table = { .entries = NULL,
					   .mask = 0,
					   .used = 0 };

static size_t keyhash(int key)
{
	// TODO: use a hash function here
	return key;
}

static int resize(size_t nel, descriptor_table_t *table)
{
	size_t newsize;
	size_t i;
	descriptor_table_item_t *e, *newe;
	descriptor_table_item_t *oldtab = table->entries;
	descriptor_table_item_t *oldend = table->entries + table->mask + 1;

	if (nel > MAXSIZE)
		nel = MAXSIZE;
	for (newsize = MINSIZE; newsize < nel; newsize *= 2)
		;
	table->entries = calloc(newsize, sizeof *table->entries);
	if (!table->entries) {
		table->entries = oldtab;
		return 0;
	}
	table->mask = newsize - 1;
	if (!oldtab)
		return 1;
	for (e = oldtab; e < oldend; e++)
		if (e->occupied) {
			for (i = keyhash(e->key);; ++i) {
				newe = table->entries + (i & table->mask);
				if (!newe->occupied)
					break;
			}
			*newe = *e;
		}
	free(oldtab);
	return 1;
}

static descriptor_table_item_t *lookup(int key, size_t hash,
				       descriptor_table_t *table)
{
	size_t i;
	descriptor_table_item_t *e;

	for (i = hash;; ++i) {
		e = table->entries + (i & table->mask);
		if (!e->occupied || e->key == key)
			break;
	}
	return e;
}

static bool insert(descriptor_table_entry_t entry, int fd,
		   descriptor_table_t *table)
{
	if (!table->entries) {
		if (!resize(MINSIZE, table)) {
			return false;
		}
	}

	size_t hash = keyhash(fd);
	descriptor_table_item_t *e = lookup(fd, hash, table);

	e->entry = entry;
	if (!e->occupied) {
		e->key = fd;
		e->occupied = true;
		if (++table->used > table->mask - table->mask / 4) {
			if (!resize(2 * table->used, table)) {
				table->used--;
				e->occupied = false;
				return false;
			}
		}
	}
	return true;
}

static bool get(int fd, descriptor_table_entry_t **entry,
		descriptor_table_t *table)
{
	if (!table->entries) {
		return false;
	}

	size_t hash = keyhash(fd);
	descriptor_table_item_t *e = lookup(fd, hash, table);
	if (e->occupied) {
		*entry = &e->entry;
		return true;
	} else {
		return false;
	}
}

static bool remove(int fd, descriptor_table_entry_t *entry,
		   descriptor_table_t *table)
{
	if (!table->entries) {
		return false;
	}

	size_t hash = keyhash(fd);
	size_t i;
	descriptor_table_item_t *e;
	for (i = hash;; ++i) {
		e = table->entries + (i & table->mask);
		if (!e->occupied || e->key == fd)
			break;
	}

	if (e->occupied) {
		*entry = e->entry;
		e->occupied = false;

		// Search for any occupied entries which would be lost (due to
		// an interrupted linear probe) if we left this one unoccupied
		// and move them as necessary.
		i = i & table->mask;
		size_t j = i;
		while (true) {
			j = (j + 1) & table->mask;
			e = table->entries + j;
			if (!e->occupied)
				break;
			size_t k = keyhash(e->key) & table->mask;
			if (i <= j) {
				if ((i < k) && (k <= j))
					continue;
			} else if ((i < k) || (k <= j)) {
				continue;
			}
			table->entries[i] = *e;
			e->occupied = false;
			i = j;
		}

		// If the load factor has dropped below 25%, shrink the table to
		// reduce memory footprint.
		if (--table->used < table->mask / 4) {
			resize(table->mask / 2, table);
		}

		return true;
	} else {
		return false;
	}
}

bool descriptor_table_insert(descriptor_table_entry_t entry, int *fd)
{
	if (wasi_preview1_adapter_open_badfd(fd)) {
		if (insert(entry, *fd, &global_table)) {
			return true;
		} else {
			if (!wasi_preview1_adapter_close_badfd(*fd)) {
				abort();
			}
			*fd = -1;
			return false;
		}
	} else {
		return false;
	}
}

bool descriptor_table_get_ref(int fd, descriptor_table_entry_t **entry)
{
	return get(fd, entry, &global_table);
}

bool descriptor_table_remove(int fd, descriptor_table_entry_t *entry)
{
	if (remove(fd, entry, &global_table)) {
		if (!wasi_preview1_adapter_close_badfd(fd)) {
			abort();
		}
		return true;
	} else {
		return false;
	}
}
