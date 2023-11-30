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

int close(int fd) {
    // Scan the preopen fds before making any changes.
    __wasilibc_populate_preopens();

    descriptor_table_variant_t variant;
    if (descriptor_table_remove(fd, &variant)) {
        switch (variant.tag) {
        case DESCRIPTOR_TABLE_VARIANT_TCP_NEW:
        case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTING:            
            wasi_sockets_0_2_0_rc_2023_10_18_tcp_tcp_socket_drop_own(variant.value.tcp_new.socket);
            break;
            
        case DESCRIPTOR_TABLE_VARIANT_TCP_ERROR:
            wasi_sockets_0_2_0_rc_2023_10_18_tcp_tcp_socket_drop_own(variant.value.tcp_error.socket.socket);
            break;
            
        case DESCRIPTOR_TABLE_VARIANT_TCP_CONNECTED:
            wasi_io_0_2_0_rc_2023_10_18_streams_input_stream_drop_own(variant.value.tcp_connected.rx);            
            wasi_io_0_2_0_rc_2023_10_18_streams_output_stream_drop_own(variant.value.tcp_connected.tx);
            wasi_sockets_0_2_0_rc_2023_10_18_tcp_tcp_socket_drop_own(variant.value.tcp_connected.socket.socket);
            break;

        default:
            errno = EBADF;
            return -1;
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
