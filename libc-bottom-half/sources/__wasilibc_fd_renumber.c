#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

#include <descriptor_table.h>

int __wasilibc_fd_renumber(int fd, int newfd) {
    // Scan the preopen fds before making any changes.
    __wasilibc_populate_preopens();

    __wasi_errno_t error = __wasi_fd_renumber(fd, newfd);
    if (error != 0) {
        errno = error;
        return -1;
    }
    return 0;
}

void drop_tcp_socket(tcp_socket_t socket) {
    switch (socket.state_tag) {
    case TCP_SOCKET_STATE_UNBOUND:
    case TCP_SOCKET_STATE_BOUND:
    case TCP_SOCKET_STATE_CONNECTING:
    case TCP_SOCKET_STATE_LISTENING:
    case TCP_SOCKET_STATE_CONNECT_FAILED:
        // No additional resources to drop.
        break;
    case TCP_SOCKET_STATE_CONNECTED: {
        tcp_socket_state_connected_t connection = socket.state.connected;

        wasi_io_0_2_0_rc_2023_10_18_poll_pollable_drop_own(connection.input_pollable);
        wasi_io_0_2_0_rc_2023_10_18_poll_pollable_drop_own(connection.output_pollable);
        wasi_io_0_2_0_rc_2023_10_18_streams_input_stream_drop_own(connection.input);
        wasi_io_0_2_0_rc_2023_10_18_streams_output_stream_drop_own(connection.output);
        break;
    }
    default: /* unreachable */ abort();
    }

    wasi_io_0_2_0_rc_2023_10_18_poll_pollable_drop_own(socket.socket_pollable);
    wasi_sockets_0_2_0_rc_2023_10_18_tcp_tcp_socket_drop_own(socket.socket);
}

void drop_udp_socket(udp_socket_t socket) {
    abort(); // TODO
}

int close(int fd) {
    // Scan the preopen fds before making any changes.
    __wasilibc_populate_preopens();

    descriptor_table_entry_t entry;
    if (descriptor_table_remove(fd, &entry)) {

        switch (entry.tag)
        {
        case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
            drop_tcp_socket(entry.value.tcp_socket);
            break;
        case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
            drop_udp_socket(entry.value.udp_socket);
            break;
        default: /* unreachable */ abort();
        }
        
        return 0;
    } else {
        __wasi_errno_t error = __wasi_fd_close(fd);
        if (error != 0) {
            errno = error;
            return -1;
        }

        return 0;
    }
}

weak void __wasilibc_populate_preopens(void) {
    // This version does nothing. It may be overridden by a version which does
    // something if `__wasilibc_find_abspath` or `__wasilibc_find_relpath` are
    // used.
}
