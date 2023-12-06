#include <assert.h>
#include <errno.h>
#include <netinet/in.h>

#include <descriptor_table.h>
#include "__utils.h"

bool tcp_accept(tcp_socket_t* socket, bool client_blocking, int* out_clientfd, network_ip_socket_address_t* out_address, int* out_errno) {

    tcp_socket_state_listening_t listener;
    if (socket->state.tag == TCP_SOCKET_STATE_LISTENING) {
        listener = socket->state.listening;
    } else {
        *out_errno = EINVAL;
        return false;
    }

    tcp_borrow_tcp_socket_t socket_borrow = tcp_borrow_tcp_socket(socket->socket);

    tcp_tuple3_own_tcp_socket_own_input_stream_own_output_stream_t client_and_io;
    network_error_code_t error;
    while (!tcp_method_tcp_socket_accept(socket_borrow, &client_and_io, &error)) {
        if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
            if (socket->blocking) {
                poll_borrow_pollable_t pollable_borrow = poll_borrow_pollable(socket->socket_pollable);
                poll_method_pollable_block(pollable_borrow);
            } else {
                *out_errno = EWOULDBLOCK;
                return false;
            }
        } else {
            *out_errno = __wasi_sockets_utils__map_error(error);
            return false;
        }
    }

    tcp_own_tcp_socket_t client = client_and_io.f0;
    tcp_borrow_tcp_socket_t client_borrow = tcp_borrow_tcp_socket(client);

    poll_own_pollable_t client_pollable = tcp_method_tcp_socket_subscribe(client_borrow);

    streams_own_input_stream_t input = client_and_io.f1;
    streams_borrow_input_stream_t input_borrow = streams_borrow_input_stream(input);
    poll_own_pollable_t input_pollable = streams_method_input_stream_subscribe(input_borrow);

    streams_own_output_stream_t output = client_and_io.f2;
    streams_borrow_output_stream_t output_borrow = streams_borrow_output_stream(output);
    poll_own_pollable_t output_pollable = streams_method_output_stream_subscribe(output_borrow);

    if (out_address != NULL) {
        if (!tcp_method_tcp_socket_remote_address(client_borrow, out_address, &error)) {
            // TODO wasi-sockets: How to recover from this in a POSIX compatible way?
            abort();
        }
    }

    descriptor_table_entry_t client_entry = { .tag = DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET, .tcp_socket = {
        .socket = client,
        .socket_pollable = client_pollable,
        .blocking = client_blocking,
        .state = { .tag = TCP_SOCKET_STATE_CONNECTED, .connected = {
            .input = input,
            .input_pollable = input_pollable,
            .output = output,
            .output_pollable = output_pollable,
        } },
    } };

    if (!descriptor_table_insert(client_entry, out_clientfd)) {
        *out_errno = EMFILE;
        return false;
    }

    return true;
}

bool udp_accept(udp_socket_t* socket, bool client_blocking, int* out_clientfd, network_ip_socket_address_t* out_address, int* out_errno) {
    // UDP doesn't support accept
    *out_errno = EOPNOTSUPP;
    return false;
}


int accept(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen) {
    return accept4(socket, addr, addrlen, 0);
}

int accept4(int socket, struct sockaddr *restrict addr, socklen_t *restrict addrlen, int flags) {

    descriptor_table_entry_t* entry;
    if (!descriptor_table_get_ref(socket, &entry)) {
        errno = EBADF;
        return -1;
    }

    if (addr != NULL) {
        if (addrlen == NULL || *addrlen == 0) {
            errno = EINVAL;
            return -1;
        }
    } else {
        if (addrlen != NULL && *addrlen == 0) {
            errno = EINVAL;
            return -1;
        }
    }

    bool client_blocking = (flags & SOCK_NONBLOCK) == 0;
    // Ignore SOCK_CLOEXEC flag. That concept does not exist in WASI.

    network_ip_socket_address_t out_address;
    network_ip_socket_address_t* out_address_ptr = addr != NULL ? &out_address : NULL;

    int client_fd;
    int err;
    switch (entry->tag)
    {
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
        if (!tcp_accept(&entry->tcp_socket, client_blocking, &client_fd, out_address_ptr, &err)) {
            errno = err;
            return -1;
        }
        break;
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
        if (!udp_accept(&entry->udp_socket, client_blocking, &client_fd, out_address_ptr, &err)) {
            errno = err;
            return -1;
        }
        break;
    default:
        errno = EOPNOTSUPP;
        return -1;
    }

    if (addr != NULL) {
        assert(out_address_ptr != NULL);

        if (!__wasi_sockets_utils__format_address(&out_address, addr, addrlen, &err)) {
            abort(); // TODO wasi-sockets: validate provided buffer length _before_ doing the actual accept wasi call.
        }
    }

    return client_fd;
}
