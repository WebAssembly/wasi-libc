#include <netdb.h>
#include <stdlib.h>

_Thread_local int h_errno = 0;

int getaddrinfo(const char* restrict host, const char* restrict serv, const struct addrinfo* restrict hint, struct addrinfo** restrict res)
{
    // TODO wasi-sockets
    abort();
}

void freeaddrinfo(struct addrinfo* p)
{
    // TODO
    abort();
}

int getnameinfo(const struct sockaddr* restrict sa, socklen_t salen, char* restrict host, socklen_t hostlen, char* restrict serv, socklen_t servlen, int flags)
{
    // TODO wasi-sockets
    abort();
}

struct hostent* gethostbyname(const char* name)
{
    // TODO
    abort();
}

struct hostent* gethostbyaddr(const void* addr, socklen_t len, int type)
{
    // TODO
    abort();
}

const char* hstrerror(int err)
{
    // TODO
    abort();
}

struct servent* getservbyname(const char* name, const char* proto)
{
    // TODO
    abort();
}

struct servent* getservbyport(int port, const char* proto)
{
    // TODO
    abort();
}

struct protoent* getprotobyname(const char* name)
{
    // TODO
    abort();
}
