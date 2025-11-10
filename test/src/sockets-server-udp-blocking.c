#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
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

void run_udp_server() {
    // Prepare server socket
    int server_port = 4001;
    // Use blocking sockets
    int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    // Bind server to socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);
    TEST(bind(server_socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) != -1);

    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);

    // Server waits for input and echoes message back to client
    // The server shuts down after the client closes the connection
    int bytes_read = 0;
    char buffer[BUFSIZE];

    while (bytes_read <= 0) {
        bytes_read = recvfrom(server_socket_fd, buffer, BUFSIZE, 0, (struct sockaddr*)&client_address, &address_len);
        if (bytes_read > 0)
            sendto(server_socket_fd, buffer, bytes_read, 0, (struct sockaddr*)&client_address, address_len);
    }

    TEST(bytes_read > 0);

    close(server_socket_fd);
}

int main()
{
    run_udp_server();

    return t_status;
}
