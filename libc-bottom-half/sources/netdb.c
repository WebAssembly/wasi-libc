#include <netdb.h>
#include <errno.h>
#include <stdlib.h>

#include <wasi/sockets_utils.h>

_Thread_local int h_errno = 0;

static int map_error(ip_name_lookup_error_code_t error)
{
	switch (error) {
	case NETWORK_ERROR_CODE_OUT_OF_MEMORY:
		return EAI_MEMORY;
	case NETWORK_ERROR_CODE_NAME_UNRESOLVABLE:
		return EAI_NONAME;
	case NETWORK_ERROR_CODE_TEMPORARY_RESOLVER_FAILURE:
		return EAI_AGAIN;
	case NETWORK_ERROR_CODE_PERMANENT_RESOLVER_FAILURE:
		return EAI_FAIL;

	default:
		errno = __wasi_sockets_utils__map_error(error);
		return EAI_SYSTEM;
	}
}

static int add_addr(ip_name_lookup_option_ip_address_t address,
		    const struct addrinfo *restrict hint,
		    struct addrinfo **restrict current,
		    struct addrinfo **restrict res)
{
	int family;
	struct sockaddr *addr;
	socklen_t addrlen;
	switch (address.val.tag) {
	case NETWORK_IP_ADDRESS_IPV4: {
		if (hint && hint->ai_family != AF_UNSPEC &&
		    hint->ai_family != AF_INET) {
			return 0;
		}

		network_ipv4_address_t ip = address.val.val.ipv4;

		family = PF_INET;
		addrlen = sizeof(struct sockaddr_in);
		addr = malloc(addrlen);
		if (addr == NULL) {
			freeaddrinfo(*res);
			return EAI_MEMORY;
		}

		struct sockaddr_in sockaddr = {
			.sin_family = AF_INET,
			.sin_port = 0,
			.sin_addr = { .s_addr = ip.f0 | (ip.f1 << 8) |
						(ip.f2 << 16) | (ip.f3 << 24) },
		};
		memcpy(addr, &sockaddr, addrlen);
		break;
	}
	case NETWORK_IP_ADDRESS_IPV6: {
		if (hint && hint->ai_family != AF_UNSPEC &&
		    hint->ai_family != AF_INET6) {
			return 0;
		}

		network_ipv6_address_t ip = address.val.val.ipv6;

		family = PF_INET6;
		addrlen = sizeof(struct sockaddr_in6);
		addr = malloc(addrlen);
		if (addr == NULL) {
			freeaddrinfo(*res);
			return EAI_MEMORY;
		}

		struct sockaddr_in6 sockaddr = {
		    .sin6_family = AF_INET6,
		    .sin6_port = 0,
		    .sin6_addr = {
			.s6_addr = {
			    ip.f0 >> 8,
			    ip.f0 & 0xFF,
			    ip.f1 >> 8,
			    ip.f1 & 0xFF,
			    ip.f2 >> 8,
			    ip.f2 & 0xFF,
			    ip.f3 >> 8,
			    ip.f3 & 0xFF,
			    ip.f4 >> 8,
			    ip.f4 & 0xFF,
			    ip.f5 >> 8,
			    ip.f5 & 0xFF,
			    ip.f6 >> 8,
			    ip.f6 & 0xFF,
			    ip.f7 >> 8,
			    ip.f7 & 0xFF,
			} },
		    .sin6_flowinfo = 0,
		    .sin6_scope_id = 0,
		};
		memcpy(addr, &sockaddr, addrlen);
		break;
	}
	default: /* unreachable */
		abort();
	}

	struct addrinfo *result = malloc(sizeof(struct addrinfo));
	if (result == NULL) {
		freeaddrinfo(*res);
		return EAI_MEMORY;
	}

	*result = (struct addrinfo){
		.ai_family = family,
		.ai_flags = 0,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0,
		.ai_addrlen = addrlen,
		.ai_addr = addr,
		.ai_canonname = NULL,
		.ai_next = NULL,
	};

	if (*current) {
		(*current)->ai_next = result;
		*current = result;
	} else {
		*current = result;
		*res = result;
	}

        return 0;
}

int getaddrinfo(const char *restrict host, const char *restrict serv,
		const struct addrinfo *restrict hint,
		struct addrinfo **restrict res)
{
	if (host == NULL) {
		host = "localhost";
	}

	*res = NULL;
	struct addrinfo *current = NULL;
	wasip2_string_t name = { .ptr = (uint8_t *)host, .len = strlen(host) };
	ip_name_lookup_own_resolve_address_stream_t stream;
	ip_name_lookup_error_code_t error;
	if (ip_name_lookup_resolve_addresses(
		    __wasi_sockets_utils__borrow_network(), &name, &stream,
		    &error)) {
		ip_name_lookup_borrow_resolve_address_stream_t stream_borrow =
			ip_name_lookup_borrow_resolve_address_stream(stream);
		while (true) {
			ip_name_lookup_option_ip_address_t address;
			if (ip_name_lookup_method_resolve_address_stream_resolve_next_address(
				    stream_borrow, &address, &error)) {
				if (address.is_some) {
					int error = add_addr(address, hint,
							     &current, res);
					if (error) {
						return error;
					}
				} else {
					return 0;
				}
			} else if (error == NETWORK_ERROR_CODE_WOULD_BLOCK) {
				ip_name_lookup_own_pollable_t pollable =
					ip_name_lookup_method_resolve_address_stream_subscribe(
						stream_borrow);
				poll_borrow_pollable_t pollable_borrow =
					poll_borrow_pollable(pollable);
				poll_method_pollable_block(pollable_borrow);
				poll_pollable_drop_own(pollable);
			} else {
				freeaddrinfo(*res);
				return map_error(error);
			}
		}
	} else {
		return map_error(error);
	}
}

void freeaddrinfo(struct addrinfo *p)
{
	while (p) {
		struct addrinfo *next = p->ai_next;
		free(p->ai_addr);
		free(p);
		p = next;
	}
}

int getnameinfo(const struct sockaddr *restrict sa, socklen_t salen,
		char *restrict host, socklen_t hostlen, char *restrict serv,
		socklen_t servlen, int flags)
{
	// TODO wasi-sockets
	abort();
}

struct hostent *gethostbyname(const char *name)
{
	// TODO wasi-sockets
	return NULL;
}

struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
	// TODO wasi-sockets
	return NULL;
}

const char *hstrerror(int err)
{
	// TODO wasi-sockets
	return "hstrerror: TODO";
}

struct servent *getservbyname(const char *name, const char *proto)
{
	// TODO wasi-sockets
	return NULL;
}

struct servent *getservbyport(int port, const char *proto)
{
	// TODO wasi-sockets
	return NULL;
}

struct protoent *getprotobyname(const char *name)
{
	// TODO wasi-sockets
	return NULL;
}
