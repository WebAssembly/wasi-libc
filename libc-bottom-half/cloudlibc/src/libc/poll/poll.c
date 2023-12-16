// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>

static int poll_preview1(struct pollfd *fds, size_t nfds, int timeout) {
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
    if (!created_events) {
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

#ifdef __wasilibc_use_preview2
#include <descriptor_table.h>

typedef struct {
    poll_own_pollable_t pollable;
    struct pollfd* pollfd;
    tcp_socket_t* socket;
    short events;
} state_t;

static int poll_preview2(struct pollfd* fds, size_t nfds, int timeout)
{
    int event_count = 0;
    for (size_t i = 0; i < nfds; ++i) {
        fds[i].revents = 0;
    }

    size_t max_pollables = (2 * nfds) + 1;
    state_t states[max_pollables];
    size_t state_index = 0;
    for (size_t i = 0; i < nfds; ++i) {
        struct pollfd* pollfd = fds + i;
        descriptor_table_entry_t* entry;
        if (descriptor_table_get_ref(pollfd->fd, &entry)) {
            switch (entry->tag) {
            case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET: {
                tcp_socket_t* socket = &(entry->tcp_socket);
                switch (socket->state.tag) {
                case TCP_SOCKET_STATE_CONNECTING: {
                    if ((pollfd->events & (POLLRDNORM | POLLWRNORM)) != 0) {
                        states[state_index++] = (state_t) {
                            .pollable = socket->socket_pollable,
                            .pollfd = pollfd,
                            .socket = socket,
                            .events = pollfd->events
                        };
                    }
                    break;
                }

                case TCP_SOCKET_STATE_CONNECTED: {
                    if ((pollfd->events & POLLRDNORM) != 0) {
                        states[state_index++] = (state_t) {
                            .pollable = socket->state.connected.input_pollable,
                            .pollfd = pollfd,
                            .socket = socket,
                            .events = POLLRDNORM
                        };
                    }
                    if ((pollfd->events & POLLWRNORM) != 0) {
                        states[state_index++] = (state_t) {
                            .pollable = socket->state.connected.output_pollable,
                            .pollfd = pollfd,
                            .socket = socket,
                            .events = POLLWRNORM
                        };
                    }
                    break;
                }

                case TCP_SOCKET_STATE_CONNECT_FAILED: {
                    if (pollfd->revents == 0) {
                        ++event_count;
                    }
                    pollfd->revents |= pollfd->events;
                    break;
                }

                default:
                    errno = ENOTSUP;
                    return -1;
                }
                break;
            }

            default:
                // TODO wasi-sockets: UDP
                errno = ENOTSUP;
                return -1;
            }
        } else {
            abort();
        }
    }

    if (event_count > 0 && timeout != 0) {
        return event_count;
    }

    poll_borrow_pollable_t pollables[state_index + 1];
    for (size_t i = 0; i < state_index; ++i) {
        pollables[i] = poll_borrow_pollable(states[i].pollable);
    }

    poll_own_pollable_t timeout_pollable;
    size_t pollable_count = state_index;
    if (timeout >= 0) {
        timeout_pollable = monotonic_clock_subscribe_duration(((monotonic_clock_duration_t)timeout) * 1000000);
        pollables[pollable_count++] = poll_borrow_pollable(timeout_pollable);
    }

    poll_list_u32_t ready;
    poll_list_borrow_pollable_t list = { .ptr = (poll_borrow_pollable_t*)&pollables, .len = pollable_count };
    poll_poll(&list, &ready);

    for (size_t i = 0; i < ready.len; ++i) {
        size_t index = ready.ptr[i];
        if (index < state_index) {
            state_t* state = &states[index];
            if (state->socket->state.tag == TCP_SOCKET_STATE_CONNECTING) {
                tcp_borrow_tcp_socket_t borrow = tcp_borrow_tcp_socket(state->socket->socket);
                tcp_tuple2_own_input_stream_own_output_stream_t tuple;
                tcp_error_code_t error;
                if (tcp_method_tcp_socket_finish_connect(borrow, &tuple, &error)) {
                    streams_borrow_input_stream_t input_stream_borrow = streams_borrow_input_stream(tuple.f0);
                    streams_own_pollable_t input_pollable = streams_method_input_stream_subscribe(input_stream_borrow);
                    streams_borrow_output_stream_t output_stream_borrow = streams_borrow_output_stream(tuple.f1);
                    streams_own_pollable_t output_pollable = streams_method_output_stream_subscribe(output_stream_borrow);
                    state->socket->state = (tcp_socket_state_t) {
                        .tag = TCP_SOCKET_STATE_CONNECTED,
                        .connected = {
                            .input_pollable = input_pollable,
                            .input = tuple.f0,
                            .output_pollable = output_pollable,
                            .output = tuple.f1,
                        }
                    };
                    if (state->pollfd->revents == 0) {
                        ++event_count;
                    }
                    state->pollfd->revents |= state->events;
                } else if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
                    // No events yet -- application will need to poll again
                } else {
                    state->socket->state = (tcp_socket_state_t) {
                        .tag = TCP_SOCKET_STATE_CONNECT_FAILED,
                        .connect_failed = {
                            .error_code = error,
                        }
                    };
                    if (state->pollfd->revents == 0) {
                        ++event_count;
                    }
                    state->pollfd->revents |= state->events;
                }
            } else {
                if (state->pollfd->revents == 0) {
                    ++event_count;
                }
                state->pollfd->revents |= state->events;
            }
        }
    }

    poll_list_u32_free(&ready);

    if (timeout >= 0) {
        poll_pollable_drop_own(timeout_pollable);
    }

    return event_count;
}

int poll(struct pollfd* fds, nfds_t nfds, int timeout)
{
    bool found_socket = false;
    bool found_non_socket = false;
    for (size_t i = 0; i < nfds; ++i) {
        descriptor_table_entry_t* entry;
        if (descriptor_table_get_ref(fds[i].fd, &entry)) {
            found_socket = true;
        } else {
            found_non_socket = true;
        }
    }

    if (found_socket) {
        if (found_non_socket) {
            // We currently don't support polling a mix of non-sockets and
            // sockets here (though you can do it by using the host APIs
            // directly), and we probably won't until we've migrated entirely to
            // WASI Preview 2.
            errno = ENOTSUP;
            return -1;
        }

        return poll_preview2(fds, nfds, timeout);
    } else if (found_non_socket) {
        return poll_preview1(fds, nfds, timeout);
    } else if (timeout >= 0) {
        return poll_preview2(fds, nfds, timeout);
    } else {
        errno = ENOTSUP;
        return -1;
    }
}
#else // __wasilibc_use_preview2
int poll(struct pollfd* fds, nfds_t nfds, int timeout)
{
    return poll_preview1(fds, nfds, timeout);
}
#endif
