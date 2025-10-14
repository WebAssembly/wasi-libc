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

// Server must be running already as a separate executable
// (except when testing the case where connect() fails)
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

    int connect_result = connect(socket_fd, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
    if (connect_result == -1) {
        close(socket_fd);
        return;
    }

    // Client writes a message to server
    // This should succeed even in the presence of hangups,
    // since send() only fails if there's a local error
    TEST(send(socket_fd, message, len, 0) == len);

    // Set timeout for recv
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    // Client reads from server
    int32_t bytes_received = recv(socket_fd, client_buffer, len, 0);
    if (bytes_received == len) {
        // Message received should be the same as message sent
        TEST(strcmp(message, client_buffer) == 0);
    }

    // Shut down client
    close(socket_fd);
}

int main(void)
{
    test_tcp_client();

    return t_status;
}
