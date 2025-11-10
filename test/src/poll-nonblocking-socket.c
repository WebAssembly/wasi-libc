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
    // Prepare client socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    TEST(socket_fd != -1);

    int server_port = 4001;

    // Check that if a non-blocking socket is in progress, poll() indicates the right result
    socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    struct sockaddr_in sockaddr_in;
    // Doesn't matter what address we connect to, since we only attempt to connect once
    sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);
    TEST(socket_fd != -1);
    int connect_result = connect(socket_fd, (const struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in));
    TEST(connect_result == -1);

    // Poll the socket for writability
    struct pollfd poll_fd = { .fd = socket_fd, .events = POLLWRNORM, .revents = 0 };
    poll(&poll_fd, 1, 10);

    // Socket should not be writable
    int32_t error = -1;
    socklen_t len = -1;
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len) == 0);
    TEST(error == ECONNREFUSED);
    close(socket_fd);
}

int main(void)
{
    test_tcp_client();

    return t_status;
}
