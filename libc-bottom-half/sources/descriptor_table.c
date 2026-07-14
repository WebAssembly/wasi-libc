/**
 * This file defines the mapping from libc-based file descriptors to WASIp2
 * resources/structures/etc. This is a slab which is indexed by file
 * descriptors and makes allocation/deallocation relatively easy.
 */

#include "lock.h"
#include <assert.h>
#include <errno.h>
#include <stddefer.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/stdio.h>

#define MINSIZE 8

typedef struct {
  bool occupied;
  descriptor_table_entry_t entry;
} descriptor_table_item_t;

typedef struct {
  DECLARE_STRONG_LOCK(lock);
  // Dynamically allocated array of `len` entries.
  descriptor_table_item_t *entries;
  // Dynamic length of `entries`.
  size_t len;
  // Allocation hint: every entry below this index is occupied, so searches
  // for a free entry can start here. Lowered on removal, advanced on
  // allocation.
  size_t next_fd;
} descriptor_table_t;

static descriptor_table_t global_table = {0};

/// Grows `table` to have capacity for at least `needed` entries, marking all
/// newly allocated entries as unoccupied. Returns -1 and sets `errno` on
/// failure.
static int table_grow(descriptor_table_t *table, size_t needed) {
  STRONG_ASSERT_HELD(table->lock);
  if (needed <= table->len)
    return 0;
  size_t new_len = table->len == 0 ? MINSIZE : table->len;
  while (new_len < needed)
    new_len *= 2;
  descriptor_table_item_t *new_entries =
      realloc(table->entries, new_len * sizeof(descriptor_table_item_t));
  if (!new_entries) {
    errno = ENOMEM;
    return -1;
  }
  memset(new_entries + table->len, 0,
         (new_len - table->len) * sizeof(descriptor_table_item_t));
  table->entries = new_entries;
  table->len = new_len;
  return 0;
}

/**
 * Allocates a new `descriptor_table_entry_t` in the `table` provided.
 *
 * Copies `entry` into the table and returns the lowest unoccupied integer
 * descriptor.
 *
 * Returns -1 on failure and sets `errno`.
 */
static int table_allocate(descriptor_table_t *table,
                          descriptor_table_entry_t entry) {
  STRONG_ASSERT_HELD(table->lock);
  size_t fd = table->next_fd;
  while (fd < table->len && table->entries[fd].occupied)
    fd++;
  if (fd == table->len && table_grow(table, table->len + 1) < 0)
    return -1;

  table->entries[fd].occupied = true;
  table->entries[fd].entry = entry;
  table->next_fd = fd + 1;
  return fd;
}

/**
 * Looks up `fd` within the provided `table`.
 *
 * Returns 0 on success and fills in `entry` with the located entry.
 *
 * Returns -1 on failure and sets `errno`.
 */
static descriptor_table_entry_t *table_lookup(descriptor_table_t *table,
                                              int fd) {
  STRONG_ASSERT_HELD(table->lock);
  if (fd < 0 || (size_t)fd >= table->len) {
    errno = EBADF;
    return NULL;
  }

  descriptor_table_item_t *table_entry = &table->entries[fd];
  if (!table_entry->occupied) {
    errno = EBADF;
    return NULL;
  }

  return &table_entry->entry;
}

/**
 * Removes `fd` within the provided `table`.
 *
 * Returns 0 on success and fills in `entry` with the contents of the entry
 * before removal.
 *
 * Returns -1 on failure and sets `errno`.
 */
static int table_remove(descriptor_table_t *table, int fd,
                        descriptor_table_entry_t *ret) {
  STRONG_ASSERT_HELD(table->lock);
  if (fd < 0 || (size_t)fd >= table->len) {
    errno = EBADF;
    return -1;
  }

  descriptor_table_item_t *table_entry = &table->entries[fd];
  if (!table_entry->occupied) {
    errno = EBADF;
    return -1;
  }

  *ret = table_entry->entry;
  table_entry->occupied = false;
  if ((size_t)fd < table->next_fd)
    table->next_fd = fd;

  return 0;
}

static void clear(descriptor_table_t *table) {
  STRONG_ASSERT_HELD(table->lock);
  for (size_t i = 0; i < table->len; ++i) {
    descriptor_table_item_t *table_entry = &table->entries[i];
    if (table_entry->occupied) {
      descriptor_table_entry_dec(table_entry->entry);
    }
  }
  if (table->entries)
    free(table->entries);
  table->entries = NULL;
  table->next_fd = 0;
  table->len = 0;
}

static bool stdio_initialized = false;

static int init_stdio() {
  stdio_initialized = true;
  return __wasilibc_init_stdio();
}

#ifdef NDEBUG
static void live_descriptors_inc() {}
static void live_descriptors_dec() {}
#else
#include <stdio.h>

static unsigned live_descriptors = 0;

static void live_descriptors_inc() { live_descriptors += 1; }

static void live_descriptors_dec() {
  assert(live_descriptors > 0);
  live_descriptors -= 1;
}

void __wasilibc_assert_no_descriptor_leaks() {
  if (!stdio_initialized) {
    assert(live_descriptors == 0);
    return;
  }

  unsigned stdio_open = 0;
  unsigned closed = 0;
  for (size_t i = 0; i < global_table.len; i++) {
    descriptor_table_item_t *table_entry = &global_table.entries[i];
    if (table_entry->occupied) {
      if (i < 3) {
        stdio_open++;
      } else {
        int rc = descriptor_table_remove(i);
        assert(rc == 0);
        closed++;
      }
    }
  }

  if (live_descriptors == stdio_open)
    return;
  fprintf(stderr, "live_descriptors: %u\n", live_descriptors);
  fprintf(stderr, "closed at end: %u\n", closed);
  fprintf(stderr, "num stdio: %u\n", stdio_open);
  fprintf(stderr,
          "ERROR: detected a fd leak (live descriptors != num stdio)\n");
  __builtin_trap();
}
#endif

static int descriptor_table_insert_entry(descriptor_table_entry_t entry) {
  STRONG_ASSERT_HELD(global_table.lock);

  assert(entry.data->cnt > 0);
  int fd = table_allocate(&global_table, entry);
  if (fd < 0)
    goto error;
  return fd;
error:
  descriptor_table_entry_dec(entry);
  return -1;
}

int descriptor_table_insert(descriptor_table_entry_t entry) {
  assert(entry.data->cnt == 0);
  entry.data->cnt = 1;
  live_descriptors_inc();

  if (!stdio_initialized && init_stdio() < 0) {
    descriptor_table_entry_dec(entry);
    return -1;
  }

  STRONG_LOCK(global_table.lock);
  defer STRONG_UNLOCK(global_table.lock);

  return descriptor_table_insert_entry(entry);
}

int descriptor_table_get(int fd, descriptor_table_entry_t *entry) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;

  STRONG_LOCK(global_table.lock);
  defer STRONG_UNLOCK(global_table.lock);

  descriptor_table_entry_t *slot = table_lookup(&global_table, fd);
  if (!slot)
    return -1;
  descriptor_table_entry_inc(*slot);
  *entry = *slot;
  return 0;
}

// Some large but not too large value to allow `dup2` to dos this process.
#define MAX_DESCRIPTOR (1 << 20)

int descriptor_table_dup(int fd, enum dup_op_t op, int arg) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;

  STRONG_LOCK(global_table.lock);
  defer STRONG_UNLOCK(global_table.lock);

  descriptor_table_entry_t *entry_ptr = table_lookup(&global_table, fd);
  if (!entry_ptr)
    return -1;

  descriptor_table_entry_t entry = *entry_ptr;

  switch (op) {
  case DUP_OP_DUP:
    descriptor_table_entry_inc(entry);
    return descriptor_table_insert_entry(entry);

  case DUP_OP_DUP2:
    if (fd == arg)
      return arg;
    // fall through ...
  case DUP_OP_DUP3: {
    if (fd == arg) {
      errno = EINVAL;
      return -1;
    }
    if (arg < 0 || arg >= MAX_DESCRIPTOR) {
      errno = EBADF;
      return -1;
    }
    size_t newfd = arg;

    if (table_grow(&global_table, newfd + 1) < 0)
      return -1;

    descriptor_table_item_t *table_entry = &global_table.entries[newfd];
    descriptor_table_entry_inc(entry);

    if (table_entry->occupied) {
      descriptor_table_entry_t prev = table_entry->entry;
      table_entry->entry = entry;
      descriptor_table_entry_dec(prev);
    } else {
      table_entry->entry = entry;
      table_entry->occupied = true;
    }
    return arg;
  }

  case DUP_OP_DUPFD: {
    if (arg < 0 || arg >= MAX_DESCRIPTOR) {
      errno = EINVAL;
      return -1;
    }
    // Search for the lowest unoccupied descriptor >= `arg`, starting at the
    // allocation hint if it's already past `arg`.
    size_t minfd = arg;
    if (minfd < global_table.next_fd)
      minfd = global_table.next_fd;
    while (minfd < global_table.len && global_table.entries[minfd].occupied)
      minfd++;
    if (table_grow(&global_table, minfd + 1) < 0)
      return -1;

    descriptor_table_entry_inc(entry);
    global_table.entries[minfd].occupied = true;
    global_table.entries[minfd].entry = entry;

    return minfd;
  }

  default:
    __builtin_trap();
  }
}

int descriptor_table_remove(int fd) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;

  STRONG_LOCK(global_table.lock);
  defer STRONG_UNLOCK(global_table.lock);

  descriptor_table_entry_t entry;
  if (table_remove(&global_table, fd, &entry) < 0)
    return -1;
  descriptor_table_entry_dec(entry);
  return 0;
}

void descriptor_table_clear() {
  STRONG_LOCK(global_table.lock);
  defer STRONG_UNLOCK(global_table.lock);

  clear(&global_table);
  stdio_initialized = false;
}

void __wasilibc_descriptor_deallocate(descriptor_table_entry_t entry) {
  assert(entry.data->cnt == 0);
  int saved_errno = errno;
  entry.vtable->free(entry.data);
  errno = saved_errno;
  live_descriptors_dec();
}
