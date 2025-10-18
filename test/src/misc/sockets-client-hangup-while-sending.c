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

    // Prepare client socket
    // Use blocking sockets
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    TEST(connect(socket_fd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) != -1);

    // Set the timeout to 2 us so that it should time out while sending
    struct timeval tv = { .tv_sec = 0, .tv_usec = 2 };
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv))==0);

    // Client writes a message to server
    TEST(send(socket_fd, message, len, 0) == len);

    // Shut down client
    close(socket_fd);
}

int main(void)
{
    test_tcp_client();

    return t_status;
}
