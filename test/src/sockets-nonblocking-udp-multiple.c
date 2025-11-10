#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

// #define _DEBUG 1

#ifdef _DEBUG
#define DEBUG_PRINT(...) fprintf( stderr, __VA_ARGS__ ); fflush(stdout)
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

#define BUFSIZE 256
int MAX_CONNECTIONS = 10;

struct response {
    bool responded;
    char message[BUFSIZE];
    int32_t client_index;
};

struct request {
    bool sent;
    char message[BUFSIZE];
};

// We are done if client_incoming == client_outgoing
bool done(struct request out[MAX_CONNECTIONS], char in[MAX_CONNECTIONS][BUFSIZE]) {
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        if (out[i].message[0] == 0) {
            return false;
        }
        if (strcmp(out[i].message, in[i]) != 0) {
            DEBUG_PRINT("sent[%zu] = %s != received[%zu] = %s\n", i, out[i].message, i, in[i]);
            return false;
        }
        DEBUG_PRINT("i = %zu %s matches %s\n", i, out[i].message, in[i]);
    }
    return true;
}

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

    // Prepare client sockets
    // Use non-blocking sockets
    int client_sockets[MAX_CONNECTIONS];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        client_sockets[i] = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
        TEST(client_sockets[i] != -1);
    }

    // Prepare sockaddr_in for client
    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    // Prepare requests from clients
    struct request client_outgoing[MAX_CONNECTIONS];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        int32_t number = 0;
        getentropy(&number, sizeof(int32_t));
        sprintf(client_outgoing[i].message, "%d", number);
        client_outgoing[i].sent = false;
    }
    char client_incoming[MAX_CONNECTIONS][BUFSIZE];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        client_incoming[i][0] = 0;
    }
    struct response server_incoming[MAX_CONNECTIONS];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        server_incoming[i].responded = false;
        server_incoming[i].message[0] = 0;
    }
    struct pollfd client_pollfds[MAX_CONNECTIONS];
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
        client_pollfds[i].fd = client_sockets[i];
        client_pollfds[i].events = POLLRDNORM;
        client_pollfds[i].revents = 0;
    }

    struct sockaddr_in addresses_of_clients[MAX_CONNECTIONS];
    socklen_t address_len = sizeof(sockaddr_in);

    struct pollfd server_pollfd = { .fd = server_socket_fd, .events = POLLRDNORM, .revents = 0 };
    struct pollfd server_client_pollfds[MAX_CONNECTIONS];
    size_t next_client = 0;
    size_t server_client_index = 0;
    int32_t tries = 0;
    while (!done(client_outgoing, client_incoming) && (tries < (MAX_CONNECTIONS * 2))) {
        DEBUG_PRINT("server polling for new messages\n");

        // Server polls for new messages
        server_pollfd.events = POLLRDNORM;
        poll(&server_pollfd, 1, 100);
        if ((server_pollfd.revents & POLLRDNORM) != 0) {
            int bytes_received = recvfrom(server_socket_fd, server_incoming[server_client_index].message,
                                          BUFSIZE, 0, (struct sockaddr*)&(addresses_of_clients[server_client_index]), &address_len);
            if (bytes_received > 0) {
                server_incoming[server_client_index].client_index = server_client_index;
                server_client_index++;
            } else
                DEBUG_PRINT("Shouldn't happen: poll(server_pollfd) said there is data but recvfrom() failed to return it\n");
        }

        DEBUG_PRINT("next client sending\n");
        // Next client sends
        if (next_client < MAX_CONNECTIONS) {
            struct pollfd client_pollfd = { .fd = client_sockets[next_client], .events = POLLWRNORM, .revents = 0 };
            poll(&client_pollfd, 1, 100);
            if ((client_pollfd.events & POLLWRNORM) != 0) {
                int32_t bytes_sent = sendto(client_sockets[next_client], client_outgoing[next_client].message,
                                            strlen(client_outgoing[next_client].message) + 1, 0, (struct sockaddr*)&sockaddr_in, address_len);
                if (bytes_sent == -1) {
                    DEBUG_PRINT("client %zu failed to send, error: %s\n", next_client, strerror(errno));
                    TEST(errno==EWOULDBLOCK);
                    // Will retry on next iteration of loop
                } else {
                    DEBUG_PRINT("client %zu sent %d bytes\n", next_client, bytes_sent);
                    client_outgoing[next_client].sent = true;
                    if (next_client < MAX_CONNECTIONS)
                        next_client++;
                }
            }
        }

        DEBUG_PRINT("server echoing\n");
        // Echo messages back to clients
        server_pollfd.events = POLLWRNORM;
        poll(&server_pollfd, 1, 100);
        for (size_t i = 0; i < server_client_index; i++) {
            if (server_incoming[i].message[0] != 0 && !server_incoming[i].responded) {
                ssize_t bytes_sent = sendto(server_socket_fd, server_incoming[i].message, BUFSIZE, 0, (struct sockaddr*)&addresses_of_clients[i], address_len);
                if (bytes_sent == -1) {
                    int the_error = errno;
                    TEST(the_error==EWOULDBLOCK);
                    // Will retry on next iteration of loop
                } else {
                    DEBUG_PRINT("server echoed message %s back to client %zu\n", server_incoming[i].message, i);
                    server_incoming[i].responded = true;
                }
                break;
            }
        }

        DEBUG_PRINT("clients receiving\n");
        // Clients receive messages
        poll(client_pollfds, MAX_CONNECTIONS, 100);
        DEBUG_PRINT("======= client_pollfds: =========\n");
        for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
            DEBUG_PRINT("{ .fd = %d .events = %d .revents = %d }\n", client_pollfds[i].fd, client_pollfds[i].events, client_pollfds[i].revents);
        }
        DEBUG_PRINT("=================================\n");
        for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
            // Note: poll() will return revents == events for an unbound UDP socket;
            // so that's why we check if we already sent on this socket, and do
            // nothing if we haven't yet
            if ((client_pollfds[i].revents & POLLRDNORM) != 0) {
                DEBUG_PRINT("i = %zu, already received (%s), already sent? %s\n", i, client_incoming[i], client_outgoing[i].sent ? "yes" : "no");
                if (client_incoming[i][0] == 0 && client_outgoing[i].sent) {
                    int32_t bytes_received = recvfrom(client_pollfds[i].fd, &client_incoming[i], BUFSIZE, 0, (struct sockaddr*)&server_address, &address_len);
                    if (bytes_received != -1) {
                        DEBUG_PRINT("client %zu received message %s (%d bytes) from server\n", i, client_incoming[i], bytes_received);
                    } else {
                        DEBUG_PRINT("Shouldn't happen: poll() said there was data, but client %zu did not receive message from server: %s\n", i, strerror(errno));
                    }
                }
            }
        }
        tries++;
    }

    DEBUG_PRINT("========= exited loop ==========\n");

    TEST(done(client_outgoing, client_incoming));

    // Shut down clients
    for (size_t i = 0; i < MAX_CONNECTIONS; i++)
        close(client_sockets[i]);

    // Shut down server
    close(server_socket_fd);
}

int main(void)
{
    test_udp_client();

    return t_status;
}
