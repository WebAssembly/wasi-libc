#include <stdlib.h>
#include <netdb.h>

int getaddrinfo(const char *restrict host, const char *restrict serv, const struct addrinfo *restrict hint, struct addrinfo **restrict res)
{
    // TODO wasi-sockets
    abort();
}

void freeaddrinfo(struct addrinfo *p)
{
    // TODO wasi-sockets
    abort();
}
