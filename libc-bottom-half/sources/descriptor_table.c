/**
 * This file defines the mapping from libc-based file descriptors to WASIp2
 * resources/structures/etc. This is a slab which is indexed by file
 * descriptors and makes allocation/deallocation relatively easy.
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <wasi/descriptor_table.h>
#include <wasi/stdio.h>

#define MINSIZE 8
#define MAXSIZE ((size_t) - 1 / 2 + 1)

typedef struct {
  bool occupied;
  union {
    int next;
    descriptor_table_entry_t entry;
  };
} descriptor_table_item_t;

typedef struct {
  // Dynamically allocated array of `cap` entries.
  descriptor_table_item_t *entries;
  // Next free entry.
  size_t next;
  // Number of `entries` that are initialized.
  size_t len;
  // Dynamic length of `entries`.
  size_t cap;
} descriptor_table_t;

static descriptor_table_t global_table = {
    .entries = NULL, .next = 0, .len = 0, .cap = 0};

/**
 * Allocates a new `descriptor_table_entry_t` in the `table` provided.
 *
 * Copies `entry` into the table and returns the integer descriptor.
 *
 * Returns -1 on failure and sets `errno`.
 */
static int table_allocate(descriptor_table_t *table,
                          descriptor_table_entry_t entry) {
  // If the table is at its limit, then a new entry needs to be allocated. If
  // the table's entire allocation capacity has been reached then that must also
  // be resized.
  if (table->next == table->len) {
    if (table->len == table->cap) {
      size_t new_cap = table->cap == 0 ? MINSIZE : table->cap * 2;
      descriptor_table_item_t *new_entries =
          realloc(table->entries, new_cap * sizeof(descriptor_table_item_t));
      if (!new_entries) {
        errno = ENOMEM;
        return -1;
      }
      table->entries = new_entries;
      table->cap = new_cap;
    }
    assert(table->len < table->cap);
    table->entries[table->len].occupied = false;
    table->entries[table->len].next = table->len + 1;
    table->len++;
  }

  descriptor_table_item_t *table_entry = &table->entries[table->next];
  int ret = table->next;
  assert(!table_entry->occupied);
  table->next = table_entry->next;
  table_entry->occupied = true;
  table_entry->entry = entry;

  return ret;
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
  table_entry->next = table->next;
  table->next = fd;

  return 0;
}

static void clear(descriptor_table_t *table) {
  for (size_t i = 0; i < table->len; ++i) {
    descriptor_table_item_t *table_entry = &table->entries[i];
    if (table_entry->occupied) {
      descriptor_table_entry_dec(table_entry->entry);
    }
  }
  if (table->entries)
    free(table->entries);
  table->entries = NULL;
  table->next = 0;
  table->len = 0;
  table->cap = 0;
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

int descriptor_table_insert(descriptor_table_entry_t entry) {
  assert(entry.data->cnt == 0);
  entry.data->cnt = 1;
  live_descriptors_inc();
  if (!stdio_initialized && init_stdio() < 0)
    goto error;
  int fd = table_allocate(&global_table, entry);
  if (fd < 0)
    goto error;
  return fd;
error:
  descriptor_table_entry_dec(entry);
  return -1;
}

int descriptor_table_get(int fd, descriptor_table_entry_t *entry) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;
  descriptor_table_entry_t *slot = table_lookup(&global_table, fd);
  if (!slot)
    return -1;
  descriptor_table_entry_inc(*slot);
  *entry = *slot;
  return 0;
}

int descriptor_table_renumber(int fd, int newfd) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;
  descriptor_table_entry_t *fdentry = table_lookup(&global_table, fd);
  if (!fdentry)
    return -1;
  descriptor_table_entry_t *newfdentry = table_lookup(&global_table, newfd);
  if (!newfdentry)
    return -1;

  descriptor_table_entry_t temp = *fdentry;
  *fdentry = *newfdentry;
  *newfdentry = temp;
  if (table_remove(&global_table, fd, &temp) < 0)
    return -1;
  descriptor_table_entry_dec(temp);
  return 0;
}

int descriptor_table_remove(int fd) {
  if (!stdio_initialized && init_stdio() < 0)
    return -1;
  descriptor_table_entry_t entry;
  if (table_remove(&global_table, fd, &entry) < 0)
    return -1;
  descriptor_table_entry_dec(entry);
  return 0;
}

void descriptor_table_clear() {
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
