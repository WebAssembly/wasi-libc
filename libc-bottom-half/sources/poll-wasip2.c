#include <errno.h>
#include <poll.h>

#include <wasi/descriptor_table.h>

typedef struct {
	poll_own_pollable_t pollable;
	struct pollfd *pollfd;
	descriptor_table_entry_t *entry;
	short events;
} state_t;

int poll_wasip2(struct pollfd *fds, size_t nfds, int timeout)
{
	int event_count = 0;
	for (size_t i = 0; i < nfds; ++i) {
		fds[i].revents = 0;
	}

	size_t max_pollables = (2 * nfds) + 1;
	state_t states[max_pollables];
	size_t state_index = 0;
	for (size_t i = 0; i < nfds; ++i) {
		struct pollfd *pollfd = fds + i;
		descriptor_table_entry_t *entry;
		if (descriptor_table_get_ref(pollfd->fd, &entry)) {
			switch (entry->tag) {
			case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET: {
				tcp_socket_t *socket = &(entry->tcp_socket);
				switch (socket->state.tag) {
				case TCP_SOCKET_STATE_CONNECTING:
				case TCP_SOCKET_STATE_LISTENING: {
					if ((pollfd->events &
					     (POLLRDNORM | POLLWRNORM)) != 0) {
						states[state_index++] = (state_t){
							.pollable =
								socket->socket_pollable,
							.pollfd = pollfd,
							.entry = entry,
							.events = pollfd->events
						};
					}
					break;
				}

				case TCP_SOCKET_STATE_CONNECTED: {
					if ((pollfd->events & POLLRDNORM) !=
					    0) {
						states[state_index++] = (state_t){
							.pollable =
								socket->state
									.connected
									.input_pollable,
							.pollfd = pollfd,
							.entry = entry,
							.events = POLLRDNORM
						};
					}
					if ((pollfd->events & POLLWRNORM) !=
					    0) {
						states[state_index++] = (state_t){
							.pollable =
								socket->state
									.connected
									.output_pollable,
							.pollfd = pollfd,
							.entry = entry,
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

			case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET: {
				udp_socket_t *socket = &(entry->udp_socket);
				switch (socket->state.tag) {
				case UDP_SOCKET_STATE_UNBOUND:
				case UDP_SOCKET_STATE_BOUND_NOSTREAMS: {
					if (pollfd->revents == 0) {
						++event_count;
					}
					pollfd->revents |= pollfd->events;
					break;
				}

				case UDP_SOCKET_STATE_BOUND_STREAMING:
				case UDP_SOCKET_STATE_CONNECTED: {
					udp_socket_streams_t *streams;
					if (socket->state.tag ==
					    UDP_SOCKET_STATE_BOUND_STREAMING) {
						streams = &(
							socket->state
								.bound_streaming
								.streams);
					} else {
						streams = &(
							socket->state.connected
								.streams);
					}
					if ((pollfd->events & POLLRDNORM) !=
					    0) {
						states[state_index++] = (state_t){
							.pollable =
								streams->incoming_pollable,
							.pollfd = pollfd,
							.entry = entry,
							.events = POLLRDNORM
						};
					}
					if ((pollfd->events & POLLWRNORM) !=
					    0) {
						states[state_index++] = (state_t){
							.pollable =
								streams->outgoing_pollable,
							.pollfd = pollfd,
							.entry = entry,
							.events = POLLWRNORM
						};
					}
					break;
				}

				default:
					errno = ENOTSUP;
					return -1;
				}
				break;
			}

			default:
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
		timeout_pollable = monotonic_clock_subscribe_duration(
			((monotonic_clock_duration_t)timeout) * 1000000);
		pollables[pollable_count++] =
			poll_borrow_pollable(timeout_pollable);
	}

	wasip2_list_u32_t ready;
	poll_list_borrow_pollable_t list = {
		.ptr = (poll_borrow_pollable_t *)&pollables,
		.len = pollable_count
	};
	poll_poll(&list, &ready);

	for (size_t i = 0; i < ready.len; ++i) {
		size_t index = ready.ptr[i];
		if (index < state_index) {
			state_t *state = &states[index];
			if (state->entry->tag ==
				    DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET &&
			    state->entry->tcp_socket.state.tag ==
				    TCP_SOCKET_STATE_CONNECTING) {
				tcp_socket_t *socket =
					&(state->entry->tcp_socket);
				tcp_borrow_tcp_socket_t borrow =
					tcp_borrow_tcp_socket(socket->socket);
				tcp_tuple2_own_input_stream_own_output_stream_t
					tuple;
				tcp_error_code_t error;
				if (tcp_method_tcp_socket_finish_connect(
					    borrow, &tuple, &error)) {
					streams_borrow_input_stream_t
						input_stream_borrow =
							streams_borrow_input_stream(
								tuple.f0);
					streams_own_pollable_t input_pollable =
						streams_method_input_stream_subscribe(
							input_stream_borrow);
					streams_borrow_output_stream_t
						output_stream_borrow =
							streams_borrow_output_stream(
								tuple.f1);
					streams_own_pollable_t output_pollable =
						streams_method_output_stream_subscribe(
							output_stream_borrow);
					socket->state =
						(tcp_socket_state_t){ .tag = TCP_SOCKET_STATE_CONNECTED,
								      .connected = {
									      .input_pollable =
										      input_pollable,
									      .input =
										      tuple.f0,
									      .output_pollable =
										      output_pollable,
									      .output =
										      tuple.f1,
								      } };
					if (state->pollfd->revents == 0) {
						++event_count;
					}
					state->pollfd->revents |= state->events;
				} else if (error ==
					   NETWORK_ERROR_CODE_WOULD_BLOCK) {
					// No events yet -- application will need to poll again
				} else {
					socket->state =
						(tcp_socket_state_t){ .tag = TCP_SOCKET_STATE_CONNECT_FAILED,
								      .connect_failed = {
									      .error_code =
										      error,
								      } };
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

	wasip2_list_u32_free(&ready);

	if (timeout >= 0) {
		poll_pollable_drop_own(timeout_pollable);
	}

	return event_count;
}
