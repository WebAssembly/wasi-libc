// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <wasi/api.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>

#if defined(__wasip1__)
static int poll_impl(struct pollfd *fds, size_t nfds, int timeout) {
  // Construct events for poll().
  size_t maxevents = 2 * nfds + 1;
  __wasi_subscription_t subscriptions[maxevents];
  size_t nsubscriptions = 0;
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    if (pollfd->fd < 0)
      continue;
    bool created_events = false;
    if ((pollfd->events & POLLRDNORM) != 0) {
      __wasi_subscription_t *subscription = &subscriptions[nsubscriptions++];
      *subscription = (__wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .u.tag = __WASI_EVENTTYPE_FD_READ,
          .u.u.fd_read.file_descriptor = pollfd->fd,
      };
      created_events = true;
    }
    if ((pollfd->events & POLLWRNORM) != 0) {
      __wasi_subscription_t *subscription = &subscriptions[nsubscriptions++];
      *subscription = (__wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .u.tag = __WASI_EVENTTYPE_FD_WRITE,
          .u.u.fd_write.file_descriptor = pollfd->fd,
      };
      created_events = true;
    }

    // As entries are decomposed into separate read/write subscriptions,
    // we cannot detect POLLERR, POLLHUP and POLLNVAL if POLLRDNORM and
    // POLLWRNORM are not specified. Disallow this for now.
    // Ignore fd entries that have no events requested.
    if (!created_events && pollfd->events != 0) {
      errno = ENOSYS;
      return -1;
    }
  }

  // Create extra event for the timeout.
  if (timeout >= 0) {
    __wasi_subscription_t *subscription = &subscriptions[nsubscriptions++];
    *subscription = (__wasi_subscription_t){
        .u.tag = __WASI_EVENTTYPE_CLOCK,
        .u.u.clock.id = __WASI_CLOCKID_REALTIME,
        .u.u.clock.timeout = (__wasi_timestamp_t)timeout * 1000000,
    };
  }

  // Execute poll().
  size_t nevents;
  __wasi_event_t events[nsubscriptions];
  __wasi_errno_t error =
      __wasi_poll_oneoff(subscriptions, events, nsubscriptions, &nevents);
  if (error != 0) {
    // WASI's poll requires at least one subscription, or else it returns
    // `EINVAL`. Since a `poll` with nothing to wait for is valid in POSIX,
    // return `ENOTSUP` to indicate that we don't support that case.
    //
    // Wasm has no signal handling, so if none of the user-provided `pollfd`
    // elements, nor the timeout, led us to producing even one subscription
    // to wait for, there would be no way for the poll to wake up. WASI
    // returns `EINVAL` in this case, but for users of `poll`, `ENOTSUP` is
    // more likely to be understood.
    if (nsubscriptions == 0)
      errno = ENOTSUP;
    else
      errno = error;
    return -1;
  }

  // Clear revents fields.
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    pollfd->revents = 0;
  }

  // Set revents fields.
  for (size_t i = 0; i < nevents; ++i) {
    const __wasi_event_t *event = &events[i];
    if (event->type == __WASI_EVENTTYPE_FD_READ ||
        event->type == __WASI_EVENTTYPE_FD_WRITE) {
      struct pollfd *pollfd = (struct pollfd *)(uintptr_t)event->userdata;
      if (event->error == __WASI_ERRNO_BADF) {
        // Invalid file descriptor.
        pollfd->revents |= POLLNVAL;
      } else if (event->error == __WASI_ERRNO_PIPE) {
        // Hangup on write side of pipe.
        pollfd->revents |= POLLHUP;
      } else if (event->error != 0) {
        // Another error occurred.
        pollfd->revents |= POLLERR;
      } else {
        // Data can be read or written.
        if (event->type == __WASI_EVENTTYPE_FD_READ) {
            pollfd->revents |= POLLRDNORM;
            if (event->fd_readwrite.flags & __WASI_EVENTRWFLAGS_FD_READWRITE_HANGUP) {
              pollfd->revents |= POLLHUP;
            }
        } else if (event->type == __WASI_EVENTTYPE_FD_WRITE) {
            pollfd->revents |= POLLWRNORM;
            if (event->fd_readwrite.flags & __WASI_EVENTRWFLAGS_FD_READWRITE_HANGUP) {
              pollfd->revents |= POLLHUP;
            }
        }
      }
    }
  }

  // Return the number of events with a non-zero revents value.
  int retval = 0;
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    // POLLHUP contradicts with POLLWRNORM.
    if ((pollfd->revents & POLLHUP) != 0)
      pollfd->revents &= ~POLLWRNORM;
    if (pollfd->revents != 0)
      ++retval;
  }
  return retval;
}

#elif defined(__wasip2__)

typedef struct {
  struct pollfd *pollfd;
  short events;
} state_t;

struct poll_state_t {
  state_t *states;
  poll_borrow_pollable_t *pollables;
  size_t len;
  size_t cap;

  int event_count;
  struct pollfd *pollfd;
};

int __wasilibc_poll_add(poll_state_t *state, short events,
                        poll_borrow_pollable_t pollable) {
  if (state->len >= state->cap) {
    errno = ENOMEM;
    return -1;
  }
  state->states[state->len].pollfd = state->pollfd;
  state->states[state->len].events = events;
  state->pollables[state->len] = pollable;
  state->len += 1;
  return 0;
}

void __wasilibc_poll_ready(poll_state_t *state, short events) {
  if (events != 0) {
    if (state->pollfd->revents == 0) {
      ++state->event_count;
    }
    state->pollfd->revents |= events;
  }
}

static int poll_impl(struct pollfd *fds, size_t nfds, int timeout) {
  for (size_t i = 0; i < nfds; ++i) {
    fds[i].revents = 0;
  }

  size_t max_pollables = (2 * nfds) + 1;
  state_t states[max_pollables];
  poll_borrow_pollable_t pollables[max_pollables];

  poll_state_t state;
  state.states = states;
  state.pollables = pollables;
  state.len = 0;
  state.cap = max_pollables;
  state.event_count = 0;
  state.pollfd = fds;

  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = fds + i;
    if (pollfd->fd < 0)
      continue;
    state.pollfd = pollfd;
    descriptor_table_entry_t *entry = descriptor_table_get_ref(pollfd->fd);
    if (!entry) {
      errno = EBADF;
      return -1;
    }

    // If this descriptor has a custom registration function then
    // use that exclusively.
    if (entry->vtable->poll_register) {
      if (entry->vtable->poll_register(entry->data, &state, pollfd->events) < 0)
        return -1;
      continue;
    }

    // Without a custom registration handle read/write readiness
    // below, but everything else is unsupported.
    if (pollfd->events & ~(POLLRDNORM | POLLWRNORM)) {
      errno = EOPNOTSUPP;
      return -1;
    }

    if (pollfd->events & POLLRDNORM) {
      if (entry->vtable->get_read_stream) {
        streams_borrow_input_stream_t input;
        poll_own_pollable_t *pollable;
        if (entry->vtable->get_read_stream(entry->data, &input, NULL,
                                           &pollable) < 0)
          return -1;
        if (__wasilibc_poll_add_input_stream(&state, input, pollable) < 0)
          return -1;
      } else {
        errno = EOPNOTSUPP;
        return -1;
      }
    }

    if (pollfd->events & POLLWRNORM) {
      if (entry->vtable->get_write_stream) {
        streams_borrow_output_stream_t output;
        poll_own_pollable_t *pollable;
        if (entry->vtable->get_write_stream(entry->data, &output, NULL,
                                            &pollable) < 0)
          return -1;
        if (__wasilibc_poll_add_output_stream(&state, output, pollable) < 0)
          return -1;
      } else {
        errno = EOPNOTSUPP;
        return -1;
      }
    }
  }

  if (state.event_count > 0 && timeout != 0) {
    return state.event_count;
  }

  poll_own_pollable_t timeout_pollable;
  size_t pollable_count = state.len;
  if (timeout >= 0) {
    timeout_pollable = monotonic_clock_subscribe_duration(
        ((monotonic_clock_duration_t)timeout) * 1000000);
    pollables[pollable_count++] = poll_borrow_pollable(timeout_pollable);
  }

  wasip2_list_u32_t ready;
  poll_list_borrow_pollable_t list = {.ptr = pollables, .len = pollable_count};
  poll_poll(&list, &ready);

  for (size_t i = 0; i < ready.len; ++i) {
    size_t index = ready.ptr[i];
    if (index >= state.len)
      continue;
    state_t *ready_state = &states[index];
    state.pollfd = ready_state->pollfd;
    descriptor_table_entry_t *entry =
        descriptor_table_get_ref(ready_state->pollfd->fd);
    if (entry->vtable->poll_finish) {
      entry->vtable->poll_finish(entry->data, &state, ready_state->events);
    } else {
      __wasilibc_poll_ready(&state, ready_state->events);
    }
  }

  wasip2_list_u32_free(&ready);

  if (timeout >= 0) {
    poll_pollable_drop_own(timeout_pollable);
  }

  return state.event_count;
}

#elif defined(__wasip3__)
#include <stdlib.h> // abort

typedef struct {
  wasip3_subtask_status_t waitable;
  struct pollfd *pollfd;
} state3_t;

static int poll_impl(struct pollfd *fds, size_t nfds, int timeout) {
  for (size_t i = 0; i < nfds; ++i) {
    fds[i].revents = 0;
  }

  int return_value = 0;
  size_t max_pollables = (2 * nfds) + 1;
  state3_t states[max_pollables];
  size_t pollable_count = 0;
  wasip3_subtask_status_t timeout_subtask;

  wasip3_waitable_set_t set = wasip3_waitable_set_new();

  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = fds + i;
    if (pollfd->fd < 0)
      continue;
    descriptor_table_entry_t *entry = descriptor_table_get_ref(pollfd->fd);
    if (!entry) {
      errno = EBADF;
      return_value = -1;
      goto cleanup_and_exit;
    }

    // Without a custom registration handle read/write readiness
    // below, but everything else is unsupported.
    if (pollfd->events & ~(POLLRDNORM | POLLWRNORM)) {
      errno = EOPNOTSUPP;
      return_value = -1;
      goto cleanup_and_exit;
    }

    if (pollfd->events & POLLRDNORM) {
      if (entry->vtable->get_read_stream3) {
        filesystem_tuple2_stream_u8_future_result_void_error_code_t *stream;
        if (entry->vtable->get_read_stream3(entry->data, &stream, NULL) < 0) {
          return_value = -1;
          goto cleanup_and_exit;
        }
        wasip3_waitable_status_t status = filesystem_stream_u8_read(stream->f0, 0, 0);
        if (status != WASIP3_WAITABLE_STATUS_BLOCKED) {
          pollfd->revents |= POLLRDNORM;
          ++return_value;
        } else if (!return_value) {
          states[pollable_count].waitable = stream->f0;
          states[pollable_count].pollfd = pollfd;
          ++pollable_count;
          wasip3_waitable_join(stream->f0, set);
        }
      } else {
        errno = EOPNOTSUPP;
        return_value = -1;
        goto cleanup_and_exit;
      }
    }

    if (pollfd->events & POLLWRNORM) {
      if (entry->vtable->get_write_stream3) {
        wasip3_write_t *stream;
        if (entry->vtable->get_write_stream3(entry->data, &stream, NULL) < 0){
          return_value = -1;
          goto cleanup_and_exit;
        }
        wasip3_waitable_status_t status = filesystem_stream_u8_write(stream->output, 0, 0);
        if (status != WASIP3_WAITABLE_STATUS_BLOCKED) {
          pollfd->revents |= POLLWRNORM;
          ++return_value;
        } else if (!return_value) {
          states[pollable_count].waitable = stream->output;
          states[pollable_count].pollfd = pollfd;
          ++pollable_count;
          wasip3_waitable_join(stream->output, set);
        }
      } else {
        errno = EOPNOTSUPP;
        return_value = -1;
        goto cleanup_and_exit;
      }
    }
  }

  if (return_value) goto cleanup_and_exit;

  if (timeout >= 0) {
    timeout_subtask = monotonic_clock_wait_for(
        ((monotonic_clock_duration_t)timeout) * 1000000);
    if (WASIP3_SUBTASK_STATE(timeout_subtask) == WASIP3_SUBTASK_RETURNED) {
      goto cleanup_and_exit;
    }
    assert(WASIP3_SUBTASK_STATE(timeout_subtask) == WASIP3_SUBTASK_STARTED);
    wasip3_waitable_join(WASIP3_SUBTASK_HANDLE(timeout_subtask), set);
  }

  wasip3_event_t event;
  wasip3_waitable_set_wait(set, &event);
  return_value = 0;
  switch (event.event) {
    case WASIP3_EVENT_STREAM_READ:
      for (size_t i=0; i<pollable_count; ++i) {
        if (states[i].waitable == event.waitable) {
          states[i].pollfd->revents |= POLLRDNORM;
          ++return_value;
          break;
        }
      }
      break;
    case WASIP3_EVENT_STREAM_WRITE:
      for (size_t i=0; i<pollable_count; ++i) {
        if (states[i].waitable == event.waitable) {
          states[i].pollfd->revents |= POLLWRNORM;
          ++return_value;
          break;
        }
      }
      break;
    case WASIP3_EVENT_SUBTASK:
      assert(event.waitable == WASIP3_SUBTASK_HANDLE(timeout_subtask));
      break;
    default:
      abort();
  }
  if (timeout >= 0) {
    wasip3_waitable_join(WASIP3_SUBTASK_HANDLE(timeout_subtask), 0);
    if (return_value>0)
      wasip3_subtask_cancel(WASIP3_SUBTASK_HANDLE(timeout_subtask));
  }

  //repeat wasip3_waitable_set_poll(set, &event); ?

cleanup_and_exit:
  for (size_t i=0; i<pollable_count; ++i) {
    wasip3_waitable_join(states[i].waitable, 0);
  }
  wasip3_waitable_set_drop(set);

  return return_value;
}

#else
# error "Unknown WASI version"
#endif

int poll(struct pollfd* fds, nfds_t nfds, int timeout) {
    return poll_impl(fds, nfds, timeout);
}
