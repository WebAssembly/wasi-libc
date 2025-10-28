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

void test_udp_client() {
    // Prepare server socket
    int server_port = 4001;
    // Use non-blocking sockets
    int server_socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    // Bind server to socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    TEST(bind(server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) != -1);

    // Prepare client socket
    // Use non-blocking sockets
    int socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    TEST(socket_fd != -1);

    // Prepare sockaddr_in for client
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    char message[] = "There's gonna be a party when the wolf comes home";
    int len = strlen(message);
    char client_buffer[BUFSIZE];
    char server_buffer[BUFSIZE];

    // Write to socket
    ssize_t bytes_sent = sendto(socket_fd, message, len + 1, 0, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));

    // Receive from client
    struct sockaddr_in sockaddr_client;
    socklen_t sockaddr_client_len = sizeof(sockaddr_in);
    ssize_t bytes_received = recvfrom(server_socket_fd, server_buffer, BUFSIZE, 0, (struct sockaddr*)&sockaddr_client, &sockaddr_client_len);
    if (bytes_received == -1) {
        TEST(errno==EWOULDBLOCK);
    }

    while (bytes_received == -1) {
        struct pollfd poll_fd = { .fd = server_socket_fd, .events = POLLRDNORM, .revents = 0 };
        poll(&poll_fd, 1, 100);
        bytes_received = recvfrom(server_socket_fd, server_buffer, BUFSIZE, 0, (struct sockaddr*)&sockaddr_client, &sockaddr_client_len);
    }

    TEST(bytes_sent == len + 1);
    TEST(bytes_received == len + 1);

    // Server echoes message back to client
    bytes_sent = sendto(server_socket_fd, server_buffer, strlen(server_buffer) + 1, 0, (struct sockaddr*)&sockaddr_client, sockaddr_client_len);
    bytes_received = recvfrom(socket_fd, client_buffer, BUFSIZE, 0, (struct sockaddr*)&sockaddr_in, &sockaddr_client_len);
    if (bytes_sent == -1) {
        struct pollfd poll_fd = { .fd = server_socket_fd, .events = POLLWRNORM, .revents = 0 };
        poll(&poll_fd, 1, 100);
    }
    if (bytes_received == -1) {
        struct pollfd poll_fd = { .fd = socket_fd, .events = POLLRDNORM, .revents = 0 };
        poll(&poll_fd, 1, 100);
        // Retry
        bytes_received = recv(socket_fd, client_buffer, BUFSIZE, 0);
    }
    // If bytes_received still < 1, consider it a timeout

    TEST(bytes_sent == len + 1);
    TEST(bytes_received == len + 1);

    // Message received should be the same as message sent
    if (bytes_sent == bytes_received)
        TEST(strcmp(message, client_buffer) == 0);

    // Shut down client
    close(socket_fd);

    // Shut down server
    close(server_socket_fd);
}

int main(void)
{
    test_udp_client();

    return t_status;
}
