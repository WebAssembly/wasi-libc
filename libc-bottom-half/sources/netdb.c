#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <wasi/sockets_utils.h>

_Thread_local int h_errno = 0;

static struct servent global_serv = { 0 };

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
		    in_port_t port,
			int socktype,
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
			.sin_port = port,
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
		    .sin6_port = port,
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
		.ai_socktype = socktype,
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

static bool set_global_serv_entry(const service_entry_t *entry, const char *proto) {
	if (!entry) {
		return false; // Service not found
	}

	global_serv.s_name = entry->s_name;
	global_serv.s_port = htons(entry->port);
	global_serv.s_aliases = NULL;

	// If proto is NULL then any protocol is matched
	if ((!proto || strcmp(proto, "tcp") == 0) && entry->protocol & SERVICE_PROTOCOL_TCP) {
		global_serv.s_proto = "tcp";
	}
	else if ((!proto || strcmp(proto, "udp") == 0) && entry->protocol & SERVICE_PROTOCOL_UDP) {
		global_serv.s_proto = "udp";
	}
	else {
		return false; // Protocol not supported
	}

	return true;
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
  if (!ip_name_lookup_resolve_addresses(
        __wasi_sockets_utils__borrow_network(), &name, &stream,
        &error))
    return map_error(error);

  int ret = 0;
  ip_name_lookup_borrow_resolve_address_stream_t stream_borrow =
    ip_name_lookup_borrow_resolve_address_stream(stream);
  // The 'serv' parameter can be either a port number or a service name.
  int port = 0;
  uint16_t protocol = SERVICE_PROTOCOL_TCP;
  if (serv != NULL) {
    port = __wasi_sockets_utils__parse_port(serv);
    if (port < 0) {
      const service_entry_t *service = __wasi_sockets_utils__get_service_entry_by_name(serv);
      if (service) {
        port = service->port;
        protocol = service->protocol;
      } else {
        ret = EAI_NONAME;
      }
    }
  }

  poll_own_pollable_t pollable;
  pollable.__handle = 0;

  while (ret == 0) {
    ip_name_lookup_option_ip_address_t address;
    if (!ip_name_lookup_method_resolve_address_stream_resolve_next_address(
          stream_borrow, &address, &error)) {
      if (error != NETWORK_ERROR_CODE_WOULD_BLOCK) {
        freeaddrinfo(*res);
        ret = map_error(error);
        break;
      }

      if (pollable.__handle == 0) {
        pollable = ip_name_lookup_method_resolve_address_stream_subscribe(
              stream_borrow);
      }
      poll_method_pollable_block(poll_borrow_pollable(pollable));
      continue;
    }
    if (!address.is_some)
      break;
    if (protocol & SERVICE_PROTOCOL_TCP) {
      ret = add_addr(address, htons(port), SOCK_STREAM,
          hint, &current, res);
      if (ret)
        break;
    }
    if (protocol & SERVICE_PROTOCOL_UDP) {
      ret = add_addr(address, htons(port), SOCK_DGRAM,
          hint, &current, res);
      if (ret)
        break;
    }
  }

  if (pollable.__handle != 0)
    poll_pollable_drop_own(pollable);
  ip_name_lookup_resolve_address_stream_drop_own(stream);
  return ret;
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
        errno = EOPNOTSUPP;
        return EAI_SYSTEM;
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
	const service_entry_t *entry = __wasi_sockets_utils__get_service_entry_by_name(name);
	if (!set_global_serv_entry(entry, proto)) {
		return NULL;
	}
	return &global_serv;
}

struct servent *getservbyport(int port, const char *proto)
{
	const service_entry_t *entry = __wasi_sockets_utils__get_service_entry_by_port(htons(port));
	if (!set_global_serv_entry(entry, proto)) {
		return NULL;
	}
	return &global_serv;
}

struct protoent *getprotobyname(const char *name)
{
	// TODO wasi-sockets
	return NULL;
}
