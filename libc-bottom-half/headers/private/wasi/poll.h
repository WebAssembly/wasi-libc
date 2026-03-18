#ifndef WASI_POLL_H
#define WASI_POLL_H

#include <wasi/version.h>

#ifndef __wasip1__

#include <poll.h>
#include <wasi/api.h>

/// Opaque state that the `poll` function manages for itself.
typedef struct poll_state_t poll_state_t;

/// Indicates that the `pollfd` which `state` refers to is ready with `events`.
///
/// If called during `poll_register` then `wasi:io/poll.poll` won't be invoked,
/// and otherwise sets the `revents` field after `poll` has run.
void __wasilibc_poll_ready(poll_state_t *state, short events);

#ifdef __wasip2__

/// Adds the `pollable` to the `state` provided.
///
/// This will add a `pollable` to get passed to `wasi:io/poll.poll`. If that
/// `pollable` becomes ready the `events` specified here will be used to set
/// `revents` in the final `pollfd`.
///
/// The `state` value internally tracks which `pollfd` is being used.
int __wasilibc_poll_add(poll_state_t *state, short events,
                        poll_borrow_pollable_t pollable);

/// Helper function to lazily subscribe to an input stream and call
/// `__wasilibc_poll_add`.
static inline int
__wasilibc_poll_add_input_stream(poll_state_t *state,
                                 streams_borrow_input_stream_t input_stream,
                                 poll_own_pollable_t *pollable) {
  if (pollable->__handle == 0)
    *pollable = streams_method_input_stream_subscribe(input_stream);
  return __wasilibc_poll_add(state, POLLRDNORM,
                             poll_borrow_pollable(*pollable));
}

/// Helper function to lazily subscribe to an output stream and call
/// `__wasilibc_poll_add`.
static inline int
__wasilibc_poll_add_output_stream(poll_state_t *state,
                                  streams_borrow_output_stream_t output_stream,
                                  poll_own_pollable_t *pollable) {
  if (pollable->__handle == 0)
    *pollable = streams_method_output_stream_subscribe(output_stream);
  return __wasilibc_poll_add(state, POLLWRNORM,
                             poll_borrow_pollable(*pollable));
}

#endif // __wasip2__

#ifdef __wasip3__

typedef void (*poll_ready_t)(void *data, poll_state_t *state, wasip3_event_t *event);

/// Adds the `waitable` to the poll `state`.
///
/// When `waitable` receives an event it'll invoke `ready` with `ready_data`,
/// the poll state, and the event that happened.
int __wasilibc_poll_add(poll_state_t *state, uint32_t waitable,
                        poll_ready_t ready, void *ready_data);

#endif // __wasip3__

#endif // not(__wasip1__)

#endif // WASI_POLL_H
