#ifndef DESCRIPTOR_TABLE_H
#define DESCRIPTOR_TABLE_H

#include <wasi/api.h>

#ifndef __wasip1__
#include <wasi/poll.h>
#include <sys/stat.h>
#include <netinet/in.h>

#ifdef __wasip3__
// create an alias to distinguish the handle type in the API
typedef uint32_t waitable_t;

/**
 * This data structure represents the write end of a file
 */
typedef struct wasip3_write_t {
  filesystem_stream_u8_writer_t output;
  // contents will be filled by host (once write has an error)
  filesystem_result_void_error_code_t pending_result;
  // this task gets ready on error or eof
  wasip3_subtask_t subtask;
} wasip3_write_t;
#endif

/**
 * Operations that are required of all descriptors registered as file
 * descriptors.
 *
 * This structure is used to dynamically dispatch operations various I/O
 * objects like files and sockets. Functions pointers below can all be `NULL`
 * except for `free`. These are the implementation of the corresponding
 * functions in libc.
 *
 * All functions below communicate errors through the return value. A -1 return
 * value indicates that an error happened and that `errno` is set.
 */
typedef struct descriptor_vtable_t {
  /// Deallocates the parameter provided, closing all resources as well.
  void (*free)(void*);

  // =====================================================================
  // Generic I/O

#ifdef __wasip2__
  /// Looks up a `wasi:io/streams.input-stream` object and stores it in
  /// the first argument. If provide also stores a pointer to the internal
  /// `off_t` offset and `pollable` for this object. The returned pointers
  /// point within the descriptor itself.
  int (*get_read_stream)(void*, streams_borrow_input_stream_t*, off_t**, poll_own_pollable_t**);

  /// Same as `get_read_stream`, but for output streams.
  int (*get_write_stream)(void*, streams_borrow_output_stream_t*, off_t**, poll_own_pollable_t**);
#endif
#ifdef __wasip3__
  int (*get_read_stream3)(void*, filesystem_tuple2_stream_u8_future_result_void_error_code_t **out, off_t** off);
  int (*get_write_stream3)(void*, wasip3_write_t **write_end, off_t**);
#endif

  /// Sets the nonblocking flag for this object to the specified value.
  int (*set_blocking)(void*, bool);

  /// Implementation of `fstat` the function call, used to learn about file
  /// descriptors.
  int (*fstat)(void*, struct stat*);

  // =====================================================================
  // File-related APIs

  /// Looks up a `wasi:filesystem/types.descriptor`, if present, from this
  /// object.
  int (*get_file)(void*, filesystem_borrow_descriptor_t*);
  /// Implementation of `lseek`-the-function.
  off_t (*seek)(void*, off_t, int);
  /// Used during `unlinkat` to ensure that all internal stremas are
  /// closed before deleting the file to ensure there are no open references to
  /// it.
  void (*close_streams)(void*);
  /// Implementation of `fnctl(fd, F_GETFL)`.
  int (*fcntl_getfl)(void*);
  /// Implementation of `fnctl(fd, F_SETFL)`.
  int (*fcntl_setfl)(void*, int);
  /// Implementation of `isatty`-the-function.
  int (*isatty)(void*);

  // =====================================================================
  // Sockets-related APIs

  /// Implementation of `accept4`-the-function.
  int (*accept4)(void*, struct sockaddr *addr, socklen_t *addrlen, int flags);
  /// Implementation of `bind`-the-function.
  int (*bind)(void*, const struct sockaddr *addr, socklen_t addrlen);
  /// Implementation of `connect`-the-function.
  int (*connect)(void*, const struct sockaddr *addr, socklen_t addrlen);
  /// Implementation of `getsockname`-the-function.
  int (*getsockname)(void*, struct sockaddr *addr, socklen_t *addrlen);
  /// Implementation of `getpeername`-the-function.
  int (*getpeername)(void*, struct sockaddr *addr, socklen_t *addrlen);
  /// Implementation of `listen`-the-function.
  int (*listen)(void*, int backlog);
  /// Implementation of `recvfrom`-the-function.
  ssize_t (*recvfrom)(void*, void *buffer, size_t length, int flags,
                      struct sockaddr *addr, socklen_t *addrlen);
  /// Implementation of `sendto`-the-function.
  ssize_t (*sendto)(void*, const void *buffer, size_t length, int flags,
                    const struct sockaddr *addr, socklen_t addrlen);
  /// Implementation of `shutdown`-the-function.
  int (*shutdown)(void*, int how);
  /// Implementation of `getsockopt`-the-function.
  int (*getsockopt)(void*, int level, int optname,
                    void *optval, socklen_t *optlen);
  /// Implementation of `setsockopt`-the-function.
  int (*setsockopt)(void*, int level, int optname,
                    const void *optval, socklen_t optlen);

  // =====================================================================
  // `poll`-related APIs

  /// Invoked when this descriptor is passed as one of the objects to the
  /// `poll` function. The `wasi/poll.h` header has functions that can operate
  /// on the `state` provided. This should invoke `__wasilibc_poll_add`, for
  /// example, for any event in `events` with a pollable derived from this
  /// object.
  ///
  /// If this function is not provided then `poll` will use `get_read_stream`
  /// and `get_write_stream`, if present, to handle `POLL{RD,WR}NORM` events.
  int (*poll_register)(void*, poll_state_t *state, short events);

  /// Invoked when `poll` has already run and detected that this object was
  /// ready. The `events` provided are the same as those provided to
  /// `__wasilibc_poll_add` originally. This function should call
  /// `__wasilibc_poll_ready` for example to indicate that the underlying
  /// `pollfd` structure has some events ready.
  ///
  /// If this function is not provided then `events` will automatically
  /// be placed into the `revents` field of `pollfd`.
  int (*poll_finish)(void*, poll_state_t *state, short events);
} descriptor_vtable_t;

/// A "fat pointer" which is placed inside of the descriptor table.
typedef struct {
  /// Arbitrary descriptor-specific data passed to `vtable` function pointer.
  void *data;
  /// Definition of various operations for this descriptor.
  descriptor_vtable_t *vtable;
} descriptor_table_entry_t;

/// Inserts the `entry` provided into the descriptor table, returning the
/// integer file descriptor used to refer to it.
///
/// On failure returns -1, sets `errno`, and runs `entry`'s destructor.
int descriptor_table_insert(descriptor_table_entry_t entry);

/// Looks up a descriptor by its file descriptor.
///
/// On success returns a non-null value of the entry in the table. On failure
/// returns `NULL` and sets errno.
descriptor_table_entry_t *descriptor_table_get_ref(int fd);

/// Removes the specified file descriptor from the table.
///
/// On success returns 0 and runs the entry's destructor. On failure returns -1
/// and sets `errno`.
int descriptor_table_remove(int fd);

/// Moves `fd` to `newfd` in the descriptor table.
///
/// This will overwrite `newfd` and open up `fd` for a future file descriptor.
/// Both descriptors are required to be present. Returns 0 on success and -1 +
/// errno on failure.
int descriptor_table_renumber(int fd, int newfd);

#endif // __wasip1__

#endif // DESCRIPTOR_TABLE_H
