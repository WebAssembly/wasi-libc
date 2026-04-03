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
    // Strip POLLPRI as it is never reported in WASI.
    short events = pollfd->events & ~POLLPRI;
    bool created_events = false;
    if ((events & POLLRDNORM) != 0) {
      __wasi_subscription_t *subscription = &subscriptions[nsubscriptions++];
      *subscription = (__wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .u.tag = __WASI_EVENTTYPE_FD_READ,
          .u.u.fd_read.file_descriptor = pollfd->fd,
      };
      created_events = true;
    }
    if ((events & POLLWRNORM) != 0) {
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
    // Ignore fd entries that have no events requested (including
    // entries that only had POLLPRI which was stripped above).
    if (!created_events && events != 0) {
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

    // Strip POLLPRI, it is never reported in WASI (no OOB data).
    short events = pollfd->events & ~POLLPRI;

    // Without a custom registration handle read/write readiness
    // below, but everything else is unsupported.
    if (events & ~(POLLRDNORM | POLLWRNORM)) {
      errno = EOPNOTSUPP;
      return -1;
    }

    if (events & POLLRDNORM) {
      if (entry->vtable->get_read_stream) {
        wasi_read_t read;
        if (entry->vtable->get_read_stream(entry->data, &read) < 0)
          return -1;
        if (__wasilibc_poll_add_input_stream(&state, read.input, read.pollable) < 0)
          return -1;
      } else {
        errno = EOPNOTSUPP;
        return -1;
      }
    }

    if (events & POLLWRNORM) {
      if (entry->vtable->get_write_stream) {
        wasi_write_t write;
        if (entry->vtable->get_write_stream(entry->data, &write) < 0)
          return -1;
        if (__wasilibc_poll_add_output_stream(&state, write.output, write.pollable) < 0)
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

typedef struct {
  struct pollfd *pollfd;
  uint32_t waitable;
  // Callback/data pointer to invoke when `waitable` receives an event, and that
  // runs any completion logic and figures out what to do next.
  poll_ready_t ready;
  void *ready_data;
} state_t;

struct poll_state_t {
  state_t *states;
  size_t len;
  size_t cap;

  wasip3_waitable_set_t set;

  int event_count;
  struct pollfd *pollfd;
};

int __wasilibc_poll_add(poll_state_t *state, uint32_t waitable,
                        poll_ready_t ready, void *ready_data) {
  if (state->len >= state->cap) {
    errno = ENOMEM;
    return -1;
  }
  state->states[state->len].pollfd = state->pollfd;
  state->states[state->len].waitable = waitable;
  state->states[state->len].ready = ready;
  state->states[state->len].ready_data = ready_data;
  wasip3_waitable_join(waitable, state->set);
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
  // TODO(wasip3) this calculation of 2n+1 is a coarse approximation but not
  // necessarily accurate. This worked well for wasip2 but this should be
  // revisited for wasip3. Maybe make this a dynamically allocated array?
  size_t max_pollables = (2 * nfds) + 1;
  state_t states[max_pollables];

  poll_state_t state;
  state.set = wasip3_waitable_set_new();
  state.states = states;
  state.len = 0;
  state.cap = max_pollables;
  state.event_count = 0;
  state.pollfd = fds;

  int ret = -1;
  wasip3_subtask_t timeout_subtask = 0;

  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = fds + i;
    if (pollfd->fd < 0)
      continue;
    state.pollfd = pollfd;
    descriptor_table_entry_t *entry = descriptor_table_get_ref(pollfd->fd);
    if (!entry) {
      errno = EBADF;
      goto out;
    }

    // If this descriptor has a custom registration function then
    // use that exclusively.
    if (entry->vtable->poll_register) {
      if (entry->vtable->poll_register(entry->data, &state, pollfd->events) < 0)
        goto out;
      continue;
    }

    // Strip POLLPRI, it is never reported in WASI (no OOB data).
    short events = pollfd->events & ~POLLPRI;

    // Without a custom registration handle read/write readiness
    // below, but everything else is unsupported.
    if (events & ~(POLLRDNORM | POLLWRNORM)) {
      errno = EOPNOTSUPP;
      goto out;
    }


    if (events & POLLRDNORM) {
      // TODO(wasip3): use `get_read_stream` and use that I/O to do something.
      // Requires a lot more integration with nonblocking I/O to get that
      // working.
      errno = EOPNOTSUPP;
      goto out;
    }

    if (events & POLLWRNORM) {
      // TODO(wasip3): use `get_write_stream` to implement this (see
      // `POLLRDNORM` above).
      errno = EOPNOTSUPP;
      goto out;
    }
  }

  // If something ended up being ready during registration then zero out the
  // timeout and continue proceeding as if this is a non-blocking call.
  if (state.event_count > 0)
    timeout = 0;

  // If a timeout is specified then spawn a subtask using
  // `monotonic-clock#wait-for`. This subtask is then added to the
  // waitable-set.  If the timeout completes immediately then pretend like
  // there's no timeout and keep going.
  //
  // Note that in `out`, the exit of this function, the subtask is cleaned up
  // if it's still in-progress.
  if (timeout > 0) {
    wasip3_subtask_status_t status = monotonic_clock_wait_for(timeout * 1000000);
    if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_RETURNED) {
      timeout = 0;
    } else {
      timeout_subtask = WASIP3_SUBTASK_HANDLE(status);
      wasip3_waitable_join(timeout_subtask, state.set);
    }
  }

  // Perform the actual blocking operation, if applicable. If the timeout is
  // still nonzero at this point then it means that it's time to actually
  // block. That handles the case where the timeout is negative as no subtask
  // was spawned and this will wait indefinitely. It also handles the case of a
  // timeout where a subtask was spawned and it's in the set being waited on.
  //
  // If the timeout is 0, however, then just perform a poll-style operation to
  // see what happened.
  wasip3_event_t event;
  if (timeout != 0) {
    wasip3_waitable_set_wait(state.set, &event);
  } else {
    wasip3_waitable_set_poll(state.set, &event);
  }
  while (event.event != WASIP3_EVENT_NONE) {
    // If this event is for the timeout subtask then that's handled directly here
    // as it's not related to any fds. Upon receiving the terminal status of
    // the subtask it's no longer candidate for cancellation so it's immediately
    // dropped and zero'd out to avoid processing in the `out` label below.
    if (event.waitable == timeout_subtask) {
      assert(event.event == WASIP3_EVENT_SUBTASK);
      assert(event.code == WASIP3_SUBTASK_RETURNED);
      wasip3_subtask_drop(timeout_subtask);
      timeout_subtask = 0;
    } else {
      // If this event is not for the timeout subtask then it's something that
      // was added to the waitable-set. That's processed here directly. Search
      // through the list of `state_t` waitables, find the right one, and invoke
      // its `ready` callback. The `ready` callback will dictate what to do
      // with `event` and will call `__wasilibc_poll_ready` as appropriate.
      //
      // TODO: having to search through the list isn't great, but short of
      // having some sort of hash table or map going from waitable->state_t
      // that's the best that can be done right. At least `poll` as a POSIX
      // interface is expected to take linear time, right?
      for (size_t i = 0; i < state.len; i++) {
        state_t *p = &state.states[i];
        if (p->waitable != event.waitable)
          continue;
        state.pollfd = p->pollfd;
        // Clear the `waitable` since this event has fired and it doesn't need
        // to be join'd to 0 below. Then invoke the custom callback originally
        // added for this which will handle any necessary completion logic and
        // updating `state.pollfd` with various events.
        p->waitable = 0;
        p->ready(p->ready_data, &state, &event);
      }
    }

    // After this event has finished being processed try to get another event.
    // This will drain the waitable-set of all ready events in one invocation of
    // this function and avoid unncessary setup/teardown in `poll`, in theory.
    wasip3_waitable_set_poll(state.set, &event);
  }

  ret = state.event_count;

out:
  if (timeout_subtask != 0) {
    wasip3_subtask_cancel(timeout_subtask);
    wasip3_subtask_drop(timeout_subtask);
  }
  for (size_t i = 0; i < state.len; i++) {
    uint32_t waitable = state.states[i].waitable;
    if (waitable)
      wasip3_waitable_join(waitable, 0);
  }
  wasip3_waitable_set_drop(state.set);

  return ret;
}

#else
# error "Unknown WASI version"
#endif

// POLLPRI (exceptional/out-of-band data) is not supported in WASI.
// If all requested events across all fds are exclusively POLLPRI,
// return ENOSYS. Otherwise, strip POLLPRI and proceed.
static int validate_something_not_pollpri(struct pollfd *fds, size_t nfds) {
  bool has_pri_only = false;
  bool has_non_pri = false;
  for (size_t i = 0; i < nfds; ++i) {
    if (fds[i].fd < 0 || fds[i].events == 0)
      continue;
    if (fds[i].events & ~POLLPRI)
      has_non_pri = true;
    else
      has_pri_only = true;
  }
  if (has_pri_only && !has_non_pri) {
    errno = ENOSYS;
    return -1;
  }
  return 0;
}

int poll(struct pollfd* fds, nfds_t nfds, int timeout) {
  if (validate_something_not_pollpri(fds, nfds) < 0)
    return -1;
  for (size_t i = 0; i < nfds; ++i)
    fds[i].revents = 0;
  return poll_impl(fds, nfds, timeout);
}
