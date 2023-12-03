#include <descriptor_table.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int setsockopt(int fd, int level, int option_name, const void* option_value, socklen_t option_len)
{
    descriptor_table_variant_t variant;
    if (!descriptor_table_get(fd, &variant)) {
        errno = EBADF;
        return -1;
    }

    reactor_own_tcp_socket_t socket;
    switch (variant.tag) {
    case DESCRIPTOR_TABLE_VARIANT_TCP_NEW:
    case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTING:
        socket = variant.value.tcp_new.socket;
        break;

    case DESCRIPTOR_TABLE_VARIANT_TCP_ERROR:
        socket = variant.value.tcp_error.socket.socket;
        break;

    case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED:
        socket = variant.value.tcp_connected.socket.socket;
        break;

    default:
        errno = EBADF;
        return -1;
    }

    reactor_borrow_tcp_socket_t socket_borrow = wasi_sockets_0_2_0_rc_2023_10_18_tcp_borrow_tcp_socket(socket);

    switch (level) {
    case IPPROTO_TCP: {
        switch (option_name) {
        case TCP_NODELAY: {
            if (option_len != sizeof(int)) {
                errno = EDOM;
                return -1;
            }

            wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_error_code_t error;
            if (wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_set_no_delay(socket_borrow, *((int*)option_value) != 0, &error)) {
                return 0;
            } else {
                // TODO: map errors appropriately
                errno = EBADF;
                return -1;
            }
        }
        default: {
            errno = ENOPROTOOPT;
            return -1;
        }
        }
    }

    case SOL_SOCKET: {
        switch (option_name) {
        case SO_KEEPALIVE: {
            if (option_len != sizeof(int)) {
                errno = EDOM;
                return -1;
            }

            wasi_sockets_0_2_0_rc_2023_10_18_tcp_create_socket_error_code_t error;
            if (wasi_sockets_0_2_0_rc_2023_10_18_tcp_method_tcp_socket_set_keep_alive(socket_borrow, *((int*)option_value) != 0, &error)) {
                return 0;
            } else {
                // TODO: map errors appropriately
                errno = EBADF;
                return -1;
            }
        }
        default: {
            errno = ENOPROTOOPT;
            return -1;
        }
        }
    }

    default: {
        errno = ENOPROTOOPT;
        return -1;
    }
    }
}
