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
    // Prepare client socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    TEST(socket_fd != -1);

    // Set send timeout to 3 and let recv timeout default to 0; test that getsockopt() returns the right results
    struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };
    socklen_t len = sizeof(tv);
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv))==0);
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 0);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 3);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));

    // Set send timeout to 0 and recv timeout to 3; test that getsockopt() returns the right results
    tv.tv_sec = 0;
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv))==0);
    tv.tv_sec = 3;
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv))==0);
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 3);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 0);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));

    // Set send timeout to 1 and recv timeout to 2; test that getsockopt() returns the right results
    tv.tv_sec = 1;
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv))==0);
    tv.tv_sec = 2;
    TEST(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv))==0);
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 2);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));
    TEST(getsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, &len)==0);
    TEST(tv.tv_sec == 1);
    TEST(tv.tv_usec == 0);
    TEST(len == sizeof(tv));

    // Shut down client
    close(socket_fd);
}

int main(void)
{
    test_tcp_client();

    return t_status;
}
