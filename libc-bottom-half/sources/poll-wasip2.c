#include <errno.h>
#include <poll.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>

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

int __wasilibc_poll_add(poll_state_t *state,
                        short events,
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
  if (state->pollfd->revents == 0) {
    ++state->event_count;
  }
  state->pollfd->revents |= events;
}


int poll_wasip2(struct pollfd *fds, size_t nfds, int timeout)
{
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
                state.pollfd = pollfd;
                descriptor_table_entry_t *entry = descriptor_table_get_ref(pollfd->fd);
                if (!entry)
                    continue;

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
                    if (entry->vtable->get_read_stream(entry->data, &input, NULL, &pollable) < 0)
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
                    if (entry->vtable->get_write_stream(entry->data, &output, NULL, &pollable) < 0)
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
                pollables[pollable_count++] =
                        poll_borrow_pollable(timeout_pollable);
        }

        wasip2_list_u32_t ready;
        poll_list_borrow_pollable_t list = {
                .ptr = pollables,
                .len = pollable_count
        };
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
