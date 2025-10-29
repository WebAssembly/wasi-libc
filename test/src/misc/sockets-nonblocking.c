//! filter.py(TARGET_TRIPLE): wasm32-wasip2
//! add-flags.py(RUN): --wasi=inherit-network=y
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

int BUFSIZE = 256;

// See sockets-server.c -- must be running already as a separate executable
void test_tcp_client() {
    // Prepare server socket
    int server_port = 4001;
    // Use non-blocking sockets
    int server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    // Bind server to socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    TEST(bind(server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) != -1);

    // Listen on socket
    char buffer[BUFSIZE];
    socklen_t client_len = sizeof(struct sockaddr_in);
    int client_socket_fd = -1;
    struct sockaddr_in client_address;
    int32_t bytes_read = 0, total_bytes_read = 0;
    TEST(listen(server_socket_fd, 1) != -1);

    // Prepare client socket
    // Use non-blocking sockets
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    TEST(socket_fd != -1);

    // Prepare sockaddr_in for client
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    // Connect from client
    char message[] = "There's gonna be a party when the wolf comes home";
    int len = strlen(message);
    char client_buffer[BUFSIZE];

    // Poll the socket if connect fails
    while (client_socket_fd == -1) {
        // Client tries to connect
        if (connect(socket_fd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
            struct pollfd poll_fd = { .fd = socket_fd, .events = POLLWRNORM, .revents = 0 };
            poll(&poll_fd, 1, 100);
        }

        // Server tries to accept
        struct pollfd poll_fd = { .fd = server_socket_fd, .events = POLLRDNORM, .revents = 0 };
        int poll_result = poll(&poll_fd, 1, 100);
        if (poll_result == 1)
            client_socket_fd = accept4(server_socket_fd, (struct sockaddr*)&client_address, &client_len, SOCK_NONBLOCK);
    }

    // Socket should now be ready.
    // Client writes a message to server
    TEST(send(socket_fd, message, len, 0) == len);

    // Server receives data and echoes message back to client
    int32_t bytes_sent = 0;
    while ((bytes_read = recv(client_socket_fd, buffer, BUFSIZE, 0)) > 0) {
        total_bytes_read += bytes_read;
    }

    // Client reads from server
    ssize_t bytes_received = 0;
    ssize_t recv_result = recv(socket_fd, client_buffer, len, 0);
    if (recv_result != -1)
        bytes_received += recv_result;
    while (bytes_received < len) {
        // Client polls to check if server is ready to send
        struct pollfd poll_fd = { .fd = socket_fd, .events = POLLRDNORM, .revents = 0 };
        int poll_result = poll(&poll_fd, 1, 100);
        if (poll_result == 1)
            bytes_received += recv(socket_fd, client_buffer, len, 0);

        // Server polls to check if client is ready to receive
        if (bytes_sent != len) {
            struct pollfd poll_fd = { .fd = client_socket_fd, .events = POLLWRNORM, .revents = 0 };
            poll_result = poll(&poll_fd, 1, 100);
            if (poll_result == 1) {
                // Echo back the data received from the client
                bytes_sent += send(client_socket_fd, buffer + bytes_sent, total_bytes_read - bytes_sent, 0);
            }
        }
    }

    TEST(bytes_received == len);

    // Message received should be the same as message sent
    TEST(strcmp(message, client_buffer) == 0);

    // Server closes connection with client
    close(client_socket_fd);

    // Shut down client
    close(socket_fd);

    // Shut down server
    close(server_socket_fd);

}

int main(void)
{
    test_tcp_client();

    return t_status;
}
