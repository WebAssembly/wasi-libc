#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <wasi/descriptor_table.h>

typedef struct {
        poll_own_pollable_t pollable;
        struct pollfd *pollfd;
        descriptor_table_entry_t *entry;
        short events;
} state_t;

static void add_revents(struct pollfd *pollfd, short revents, int *event_count) {
  if (pollfd->revents == 0) {
    *event_count += 1;
  }
  pollfd->revents |= revents;
}

static void add_state(state_t *states,
                      size_t *state_index,
                      struct pollfd *pollfd,
                      descriptor_table_entry_t *entry,
                      poll_own_pollable_t pollable,
                      short events)
{
  states[*state_index] = (state_t){
    .pollable = pollable,
    .pollfd = pollfd,
    .entry = entry,
    .events = events
  };
  *state_index += 1;
}

/**
 * Adds `pollfd` to the `states` list, incrementing `state_index` for each
 * pollable added.
 *
 * This will determine, based on `pollfd`, which pollables get added to the list
 * of pollables that are checked.
 *
 * Returns -1 on error and sets `errno`.
 */
static int add_pollfd(state_t *states,
                      size_t *state_index,
                      struct pollfd *pollfd,
                      int *event_count)
{
    if (pollfd->fd < 0)
        return 0;
    descriptor_table_entry_t *entry;
    if (!descriptor_table_get_ref(pollfd->fd, &entry)) {
        errno = EBADF;
        return -1;
    }

    switch (entry->tag) {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET: {
            tcp_socket_t *socket = &(entry->tcp_socket);
            switch (socket->state.tag) {
            case TCP_SOCKET_STATE_CONNECTING:
            case TCP_SOCKET_STATE_LISTENING: {
                    if ((pollfd->events & (POLLRDNORM | POLLWRNORM)) != 0) {
                            tcp_borrow_tcp_socket_t borrow = tcp_borrow_tcp_socket(socket->socket);
                            add_state(states, state_index, pollfd, entry,
                                      tcp_method_tcp_socket_subscribe(borrow),
                                      pollfd->events);
                    }
                    break;
            }

            case TCP_SOCKET_STATE_CONNECTED: {
                    if ((pollfd->events & POLLRDNORM) != 0) {
                            streams_borrow_input_stream_t borrow =
                                streams_borrow_input_stream(socket->state.connected.input);
                            add_state(states, state_index, pollfd, entry,
                                      streams_method_input_stream_subscribe(borrow),
                                      POLLRDNORM);
                    }
                    if ((pollfd->events & POLLWRNORM) != 0) {
                            streams_borrow_output_stream_t borrow =
                                streams_borrow_output_stream(socket->state.connected.output);
                            add_state(states, state_index, pollfd, entry,
                                      streams_method_output_stream_subscribe(borrow),
                                      POLLWRNORM);
                    }
                    break;
            }

            case TCP_SOCKET_STATE_CONNECT_FAILED: {
                    add_revents(pollfd, pollfd->events, event_count);
                    break;
            }

            default:
                    errno = ENOTSUP;
                    return -1;
            }
            break;
    }

    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET: {
            udp_socket_t *socket = &(entry->udp_socket);
            switch (socket->state.tag) {
            case UDP_SOCKET_STATE_UNBOUND:
            case UDP_SOCKET_STATE_BOUND_NOSTREAMS: {
                    add_revents(pollfd, pollfd->events, event_count);
                    break;
            }

            case UDP_SOCKET_STATE_BOUND_STREAMING:
            case UDP_SOCKET_STATE_CONNECTED: {
                    udp_socket_streams_t *streams;
                    if (socket->state.tag == UDP_SOCKET_STATE_BOUND_STREAMING) {
                            streams = &socket->state.bound_streaming.streams;
                    } else {
                            streams = &socket->state.connected.streams;
                    }
                    if ((pollfd->events & POLLRDNORM) != 0) {
                            udp_borrow_incoming_datagram_stream_t borrow =
                                udp_borrow_incoming_datagram_stream(streams->incoming);
                            add_state(states, state_index, pollfd, entry,
                                      udp_method_incoming_datagram_stream_subscribe(borrow),
                                      POLLRDNORM);
                    }
                    if ((pollfd->events & POLLWRNORM) != 0) {
                            udp_borrow_outgoing_datagram_stream_t borrow =
                                udp_borrow_outgoing_datagram_stream(streams->outgoing);
                            add_state(states, state_index, pollfd, entry,
                                      udp_method_outgoing_datagram_stream_subscribe(borrow),
                                      POLLWRNORM);
                    }
                    break;
            }

            default:
                    errno = ENOTSUP;
                    return -1;
            }
            break;
    }
    case DESCRIPTOR_TABLE_ENTRY_FILE_STREAM: {
        file_stream_t *stream = &entry->stream;
        if ((pollfd->events & POLLRDNORM) != 0) {
            if (!stream->file_info.readable) {
                errno = EBADF;
                return -1;
            }
            add_state(states, state_index, pollfd, entry,
                      streams_method_input_stream_subscribe(stream->read_stream),
                      POLLRDNORM);
        }
        if ((pollfd->events & POLLWRNORM) != 0) {
            if (!stream->file_info.writable) {
                errno = EBADF;
                return -1;
            }
            add_state(states, state_index, pollfd, entry,
                      streams_method_output_stream_subscribe(stream->write_stream),
                      POLLWRNORM);
        }
        break;
    }
    // File must be open
    case DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE:
            errno = EBADF;
            return -1;
    default:
            errno = ENOTSUP;
            return -1;
    }


    return 0;
}

int poll_wasip2(struct pollfd *fds, size_t nfds, int timeout)
{
        int event_count = 0;
        for (size_t i = 0; i < nfds; ++i) {
                fds[i].revents = 0;
        }

        size_t max_pollables = (2 * nfds) + 1;
        state_t states[max_pollables];
        poll_borrow_pollable_t pollables[max_pollables];
        size_t state_index = 0;
        for (size_t i = 0; i < nfds; ++i) {
                if (add_pollfd(states, &state_index, fds + i, &event_count) < 0) {
                    event_count = -1;
                    goto cleanup_states;
                }
        }

        if (event_count > 0 && timeout != 0) {
                goto cleanup_states;
        }
        assert(state_index < max_pollables);

        for (size_t i = 0; i < state_index; ++i) {
                pollables[i] = poll_borrow_pollable(states[i].pollable);
        }

        poll_own_pollable_t timeout_pollable;
        size_t pollable_count = state_index;
        if (timeout >= 0) {
                timeout_pollable = monotonic_clock_subscribe_duration(
                        ((monotonic_clock_duration_t)timeout) * 1000000);
                pollables[pollable_count++] = poll_borrow_pollable(timeout_pollable);
        }

        wasip2_list_u32_t ready;
        poll_list_borrow_pollable_t list = {
                .ptr = pollables,
                .len = pollable_count
        };
        poll_poll(&list, &ready);

        for (size_t i = 0; i < ready.len; ++i) {
                size_t index = ready.ptr[i];
                if (index >= state_index)
                        continue;
                state_t *state = &states[index];
                if (state->entry->tag ==
                            DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET &&
                    state->entry->tcp_socket.state.tag ==
                            TCP_SOCKET_STATE_CONNECTING) {
                        tcp_socket_t *socket = &state->entry->tcp_socket;
                        tcp_borrow_tcp_socket_t borrow =
                                tcp_borrow_tcp_socket(socket->socket);
                        tcp_tuple2_own_input_stream_own_output_stream_t tuple;
                        tcp_error_code_t error;
                        if (tcp_method_tcp_socket_finish_connect(
                                    borrow, &tuple, &error)) {
                                socket->state = (tcp_socket_state_t){
                                    .tag = TCP_SOCKET_STATE_CONNECTED,
                                    .connected = {
                                        .input = tuple.f0,
                                        .output = tuple.f1,
                                    }
                                };
                                add_revents(state->pollfd, state->events, &event_count);
                        } else if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
                                // No events yet -- application will need to poll again
                        } else {
                                socket->state = (tcp_socket_state_t){
                                    .tag = TCP_SOCKET_STATE_CONNECT_FAILED,
                                    .connect_failed = { .error_code = error },
                                };
                                add_revents(state->pollfd, state->events, &event_count);
                        }
                } else {
                        add_revents(state->pollfd, state->events, &event_count);
                }

        }

        wasip2_list_u32_free(&ready);

        if (timeout >= 0) {
                poll_pollable_drop_own(timeout_pollable);
        }

cleanup_states:
        for (size_t i = 0; i < state_index; ++i) {
                poll_pollable_drop_own(states[i].pollable);
        }
        return event_count;
}
