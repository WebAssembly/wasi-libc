#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

int tcp_listen(tcp_socket_t* socket, int backlog) {

    switch (socket->state_tag) {
    case TCP_SOCKET_STATE_UNBOUND:
        // TODO: perform implicit bind and then continue with this listen operation.
        errno = EDESTADDRREQ;
        return -1;
    case TCP_SOCKET_STATE_BOUND:
        // These can initiate a connect.
        break;
    case TCP_SOCKET_STATE_LISTENING:
        // We can only update the backlog size.
        break;
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_CONNECTED:
    case TCP_SOCKET_STATE_CONNECT_FAILED:
    default:
        errno = EINVAL;
        return -1;
    }

    wasi_sockets_0_2_0_rc_2023_10_18_network_error_code_t error;
    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket->socket);

    if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_set_listen_backlog_size(socket_borrow, backlog, &error)) {
        abort(); // Our own state checks should've prevented this from happening.
    }

    if (socket->state_tag == TCP_SOCKET_STATE_LISTENING) {
        // Updating the backlog is all we had to do.
        return 0;
    }

    reactor_borrow_network_t network_borrow = __wasi_sockets_utils__borrow_network();
    if (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_start_listen(socket_borrow, &error)) {
        errno = __wasi_sockets_utils__map_error(error);
        return -1;
    }

    // Listen has successfully started. Attempt to finish it:
    while (!wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_finish_listen(socket_borrow, &error)) {
        if (error == WASI_SOCKETS_0_2_0_RC_2023_10_18_NETWORK_ERROR_CODE_WOULD_BLOCK) {
    		reactor_borrow_pollable_t pollable_borrow = wasi_io_0_2_0_rc_2023_10_18_poll_borrow_pollable(socket->socket_pollable);
    		wasi_io_0_2_0_rc_2023_10_18_poll_poll_one(pollable_borrow);
        } else {
            errno = __wasi_sockets_utils__map_error(error);
            return -1;
        }
    }

    // Listen successful.

    socket->state_tag = TCP_SOCKET_STATE_LISTENING;
    socket->state = (tcp_socket_state_t){ .listening = { /* No additional state */ } };
    return 0;
}

int udp_listen(udp_socket_t* socket, int backlog) {
    // UDP doesn't support listen
    errno = EOPNOTSUPP;
    return -1;
}

int listen(int socket, int backlog) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (backlog < 0) {
        // POSIX:
        // > If listen() is called with a backlog argument value that is
        // > less than 0, the function behaves as if it had been called
        // > with a backlog argument value of 0.
        backlog = 0;
    }

    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        return tcp_listen(&entry->value.tcp_socket, backlog);
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        return udp_listen(&entry->value.udp_socket, backlog);
    default:
        errno = EOPNOTSUPP;
        return -1;
    }
}
