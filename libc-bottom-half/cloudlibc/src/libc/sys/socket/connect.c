#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"


int tcp_connect(tcp_socket_t* socket, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address)
{
    switch (socket->state_tag) {
    case TCP_SOCKET_STATE_UNBOUND:
    case TCP_SOCKET_STATE_BOUND:
        // These can initiate a connect.
        break;
    case TCP_SOCKET_STATE_CONNECTING:
        errno = EALREADY;
        return -1;
    case TCP_SOCKET_STATE_CONNECTED:
        errno = EISCONN;
        return -1;
    case TCP_SOCKET_STATE_CONNECT_FAILED: // POSIX: "If connect() fails, the state of the socket is unspecified. Conforming applications should close the file descriptor and create a new socket before attempting to reconnect."
    case TCP_SOCKET_STATE_LISTENING:
    default:
        errno = EOPNOTSUPP;
        return -1;
    }

    wasi_sockets_0_2_0_rc_2023_10_18_network_error_code_t error;
    reactor_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket->socket);

    if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_start_connect(socket_borrow, network_borrow, address, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
    }

    // Connect has successfully started.
    socket->state_tag = TCP_SOCKET_STATE_CONNECTING;
    socket->state = (tcp_socket_state_t){ .connecting = { /* No additional state */ } };

    // Attempt to finish it:
    reactor_tuple2_own_input_stream_own_output_stream_t io;
    while (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_finish_connect(socket_borrow, &io, &error)) {
        if (error == WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_WOULD_BLOCK) {
            if (socket->blocking) {
                reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(socket->socket_pollable);
                wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
            } else {
                errno = EINPROGRESS;
                return -1;
            }
        } else {
            socket->state_tag = TCP_SOCKET_STATE_CONNECT_FAILED;
            socket->state = (tcp_socket_state_t){ .connect_failed = {
                .error_code = error,
            } };

            errno = __wasi_sockets_utils__map_error(error);
            return -1;
        }
    }

    // Connect successful.

    reactor_own_input_stream_t input = io.f0;
    reactor_borrow_input_stream_t input_borrow = wasi_io_0_2_0_rc_2023_10_18_streams_borrow_input_stream(input);
    reactor_own_pollable_t input_pollable = wasi_io_0_2_0_rc_2023_10_18_streams_method_input_stream_subscribe(input_borrow);

    reactor_own_output_stream_t output = io.f1;
    reactor_borrow_output_stream_t output_borrow = wasi_io_0_2_0_rc_2023_10_18_streams_borrow_output_stream(output);
    reactor_own_pollable_t output_pollable = wasi_io_0_2_0_rc_2023_10_18_streams_method_output_stream_subscribe(output_borrow);

    socket->state_tag = TCP_SOCKET_STATE_CONNECTED;
    socket->state = (tcp_socket_state_t){ .connected = {
        .input = input,
        .input_pollable = input_pollable,
        .output = output,
        .output_pollable = output_pollable,
    } };
    return 0;
}

int udp_connect(udp_socket_t* socket, wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t* address)
{
    // TODO: implement
    errno = EOPNOTSUPP;
    return -1;
}

int connect(int fd, const struct sockaddr* address, socklen_t len)
{
    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(fd, &entry)) {
        errno = EBADF;
        return -1;
    }

    wasi_sockets_0_2_0_rc_2023_10_18_network_ip_socket_address_t ip_address;
    int parse_err;
    if (!__wasi_sockets_utils__parse_address(address, len, &ip_address, &parse_err)) {
        errno = parse_err;
        return -1;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_connect(&entry->value.tcp_socket, &ip_address);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_connect(&entry->value.udp_socket, &ip_address);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
