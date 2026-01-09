#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/sockets_utils.h>

// For now favor the naming of wasip3 which uses the "sockets" namespace
// instead of the "network" namespace. The definitions of these types in WIT,
// however, is the same so all that's needed is some renaming here.
#ifdef __wasip2__
#define SOCKETS_ERROR_CODE_ACCESS_DENIED NETWORK_ERROR_CODE_ACCESS_DENIED
#define SOCKETS_ERROR_CODE_NOT_SUPPORTED NETWORK_ERROR_CODE_NOT_SUPPORTED
#define SOCKETS_ERROR_CODE_INVALID_ARGUMENT NETWORK_ERROR_CODE_INVALID_ARGUMENT
#define SOCKETS_ERROR_CODE_OUT_OF_MEMORY NETWORK_ERROR_CODE_OUT_OF_MEMORY
#define SOCKETS_ERROR_CODE_TIMEOUT NETWORK_ERROR_CODE_TIMEOUT
#define SOCKETS_ERROR_CODE_ADDRESS_NOT_BINDABLE                                \
  NETWORK_ERROR_CODE_ADDRESS_NOT_BINDABLE
#define SOCKETS_ERROR_CODE_ADDRESS_IN_USE NETWORK_ERROR_CODE_ADDRESS_IN_USE
#define SOCKETS_ERROR_CODE_REMOTE_UNREACHABLE                                  \
  NETWORK_ERROR_CODE_REMOTE_UNREACHABLE
#define SOCKETS_ERROR_CODE_CONNECTION_REFUSED                                  \
  NETWORK_ERROR_CODE_CONNECTION_REFUSED
#define SOCKETS_ERROR_CODE_CONNECTION_RESET NETWORK_ERROR_CODE_CONNECTION_RESET
#define SOCKETS_ERROR_CODE_CONNECTION_ABORTED                                  \
  NETWORK_ERROR_CODE_CONNECTION_ABORTED
#define SOCKETS_ERROR_CODE_DATAGRAM_TOO_LARGE                                  \
  NETWORK_ERROR_CODE_DATAGRAM_TOO_LARGE
#define SOCKETS_ERROR_CODE_INVALID_STATE NETWORK_ERROR_CODE_INVALID_STATE
#define SOCKETS_ERROR_CODE_UNKNOWN NETWORK_ERROR_CODE_UNKNOWN

#define SOCKETS_IP_ADDRESS_FAMILY_IPV4 NETWORK_IP_ADDRESS_FAMILY_IPV4
#define SOCKETS_IP_ADDRESS_FAMILY_IPV6 NETWORK_IP_ADDRESS_FAMILY_IPV6
#define SOCKETS_IP_SOCKET_ADDRESS_IPV4 NETWORK_IP_SOCKET_ADDRESS_IPV4
#define SOCKETS_IP_SOCKET_ADDRESS_IPV6 NETWORK_IP_SOCKET_ADDRESS_IPV6

typedef network_ipv4_socket_address_t sockets_ipv4_socket_address_t;
typedef network_ipv4_address_t sockets_ipv4_address_t;
typedef network_ipv6_socket_address_t sockets_ipv6_socket_address_t;
typedef network_ipv6_address_t sockets_ipv6_address_t;
typedef network_ip_socket_address_t sockets_ip_socket_address_t;
#endif

static bool global_network_initialized = false;
static const service_entry_t global_services[] = {
    {"domain", 53, SERVICE_PROTOCOL_TCP | SERVICE_PROTOCOL_UDP},
    {"ftp", 21, SERVICE_PROTOCOL_TCP},
    {"ftp-data", 20, SERVICE_PROTOCOL_TCP},
    {"ftps", 990, SERVICE_PROTOCOL_TCP},
    {"ftps-data", 989, SERVICE_PROTOCOL_TCP},
    {"http", 80, SERVICE_PROTOCOL_TCP | SERVICE_PROTOCOL_UDP},
    {"https", 443, SERVICE_PROTOCOL_TCP | SERVICE_PROTOCOL_UDP},
    {"imap", 143, SERVICE_PROTOCOL_TCP},
    {"imaps", 993, SERVICE_PROTOCOL_TCP},
    {"ntp", 123, SERVICE_PROTOCOL_TCP},
    {"pop3", 110, SERVICE_PROTOCOL_TCP},
    {"pop3s", 995, SERVICE_PROTOCOL_TCP},
    {"smtp", 25, SERVICE_PROTOCOL_TCP},
    {"ssh", 22, SERVICE_PROTOCOL_TCP},
    {"submission", 587, SERVICE_PROTOCOL_TCP},
    {"submissions", 465, SERVICE_PROTOCOL_TCP},
    {"telnet", 23, SERVICE_PROTOCOL_TCP},
    {0},
};
weak_alias(global_services, __wasi_sockets_services_db);

#ifdef __wasip2__
static network_own_network_t global_network;

network_borrow_network_t __wasi_sockets_utils__borrow_network() {
  if (!global_network_initialized) {
    global_network = instance_network_instance_network();
    global_network_initialized = true;
  }

  return network_borrow_network(global_network);
}
#endif

int __wasi_sockets_utils__map_error(sockets_error_code_t wasi_error) {
  switch (wasi_error) {
  case SOCKETS_ERROR_CODE_ACCESS_DENIED:
    return EACCES;
  case SOCKETS_ERROR_CODE_NOT_SUPPORTED:
    return EOPNOTSUPP;
  case SOCKETS_ERROR_CODE_INVALID_ARGUMENT:
    return EINVAL;
  case SOCKETS_ERROR_CODE_OUT_OF_MEMORY:
    return ENOMEM;
  case SOCKETS_ERROR_CODE_TIMEOUT:
    return ETIMEDOUT;
#ifdef __wasip2__
  case NETWORK_ERROR_CODE_CONCURRENCY_CONFLICT:
    return EALREADY;
  case NETWORK_ERROR_CODE_WOULD_BLOCK:
    return EWOULDBLOCK;
  case NETWORK_ERROR_CODE_NEW_SOCKET_LIMIT:
    return EMFILE;
#endif
  case SOCKETS_ERROR_CODE_ADDRESS_NOT_BINDABLE:
    return EADDRNOTAVAIL;
  case SOCKETS_ERROR_CODE_ADDRESS_IN_USE:
    return EADDRINUSE;
  case SOCKETS_ERROR_CODE_REMOTE_UNREACHABLE:
    return EHOSTUNREACH;
  case SOCKETS_ERROR_CODE_CONNECTION_REFUSED:
    return ECONNREFUSED;
  case SOCKETS_ERROR_CODE_CONNECTION_RESET:
    return ECONNRESET;
  case SOCKETS_ERROR_CODE_CONNECTION_ABORTED:
    return ECONNABORTED;
  case SOCKETS_ERROR_CODE_DATAGRAM_TOO_LARGE:
    return EMSGSIZE;

  case SOCKETS_ERROR_CODE_INVALID_STATE:
#ifdef __wasip2__
  case NETWORK_ERROR_CODE_NOT_IN_PROGRESS:
#endif
    abort(); // If our internal state checks are working right, these errors
             // should never show up.
    break;

#ifdef __wasip2__
  case NETWORK_ERROR_CODE_NAME_UNRESOLVABLE:
  case NETWORK_ERROR_CODE_TEMPORARY_RESOLVER_FAILURE:
  case NETWORK_ERROR_CODE_PERMANENT_RESOLVER_FAILURE:
    abort(); // These errors are specific to getaddrinfo, which should have
             // filtered these errors out before calling this generic method
    break;
#endif

  case SOCKETS_ERROR_CODE_UNKNOWN:
  default:
    return EOPNOTSUPP;
  }
}

bool __wasi_sockets_utils__parse_address(
    sockets_ip_address_family_t expected_family, const struct sockaddr *address,
    socklen_t len, sockets_ip_socket_address_t *result, int *error) {
  if (address == NULL || len < sizeof(struct sockaddr)) {
    *error = EINVAL;
    return false;
  }

  switch (expected_family) {
  case SOCKETS_IP_ADDRESS_FAMILY_IPV4: {
    if (address->sa_family != AF_INET) {
      *error = EAFNOSUPPORT;
      return false;
    }

    if (len < sizeof(struct sockaddr_in)) {
      *error = EINVAL;
      return false;
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *)address;
    unsigned ip = ipv4->sin_addr.s_addr;
    unsigned short port = ipv4->sin_port;
    *result = (sockets_ip_socket_address_t){
        .tag = SOCKETS_IP_SOCKET_ADDRESS_IPV4,
        .val = {.ipv4 =
                    {
                        .port = ntohs(port), // (port << 8) | (port >> 8),
                        .address = {ip & 0xFF, (ip >> 8) & 0xFF,
                                    (ip >> 16) & 0xFF, ip >> 24},
                    }},
    };
    return true;
  }
  case SOCKETS_IP_ADDRESS_FAMILY_IPV6: {
    if (address->sa_family != AF_INET6) {
      *error = EAFNOSUPPORT;
      return false;
    }

    if (len < sizeof(struct sockaddr_in6)) {
      *error = EINVAL;
      return false;
    }

    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)address;
    unsigned char *ip = (unsigned char *)&(ipv6->sin6_addr.s6_addr);
    unsigned short port = ipv6->sin6_port;
    *result = (sockets_ip_socket_address_t){
        .tag = SOCKETS_IP_SOCKET_ADDRESS_IPV6,
        .val = {.ipv6 = {
                    .port = ntohs(port),
                    .address =
                        {
                            (((unsigned short)ip[0]) << 8) | ip[1],
                            (((unsigned short)ip[2]) << 8) | ip[3],
                            (((unsigned short)ip[4]) << 8) | ip[5],
                            (((unsigned short)ip[6]) << 8) | ip[7],
                            (((unsigned short)ip[8]) << 8) | ip[9],
                            (((unsigned short)ip[10]) << 8) | ip[11],
                            (((unsigned short)ip[12]) << 8) | ip[13],
                            (((unsigned short)ip[14]) << 8) | ip[15],
                        },
                    // TODO wasi-sockets: do these need to be endian-reversed?
                    .flow_info = ipv6->sin6_flowinfo,
                    .scope_id = ipv6->sin6_scope_id,
                }}};
    return true;
  }
  default: /* unreachable */
    abort();
  }
}

bool __wasi_sockets_utils__output_addr_validate(
    sockets_ip_address_family_t expected_family, struct sockaddr *addr,
    socklen_t *addrlen, output_sockaddr_t *result) {
  // The address parameters must be either both null or both _not_ null.

  if (addr == NULL && addrlen == NULL) {
    *result = (output_sockaddr_t){.tag = OUTPUT_SOCKADDR_NULL, .null = {}};
    return true;

  } else if (addr != NULL && addrlen != NULL) {
    if (expected_family == SOCKETS_IP_ADDRESS_FAMILY_IPV4) {
      if (*addrlen < sizeof(struct sockaddr_in)) {
        return false;
      }

      *result = (output_sockaddr_t){.tag = OUTPUT_SOCKADDR_V4,
                                    .v4 = {
                                        .addr = (struct sockaddr_in *)addr,
                                        .addrlen = addrlen,
                                    }};
      return true;

    } else if (expected_family == SOCKETS_IP_ADDRESS_FAMILY_IPV6) {
      if (*addrlen < sizeof(struct sockaddr_in6)) {
        return false;
      }

      *result = (output_sockaddr_t){.tag = OUTPUT_SOCKADDR_V6,
                                    .v6 = {
                                        .addr = (struct sockaddr_in6 *)addr,
                                        .addrlen = addrlen,
                                    }};
      return true;

    } else {
      abort();
    }

  } else {
    return false;
  }
}

void __wasi_sockets_utils__output_addr_write(
    const sockets_ip_socket_address_t input, output_sockaddr_t *output) {
  switch (input.tag) {
  case SOCKETS_IP_SOCKET_ADDRESS_IPV4: {
    if (output->tag != OUTPUT_SOCKADDR_V4) {
      abort();
    }

    sockets_ipv4_socket_address_t input_v4 = input.val.ipv4;
    sockets_ipv4_address_t ip = input_v4.address;

    *output->v4.addrlen = sizeof(struct sockaddr_in);
    *output->v4.addr = (struct sockaddr_in){
        .sin_family = AF_INET,
        .sin_port = htons(input_v4.port),
        .sin_addr = {.s_addr =
                         ip.f0 | (ip.f1 << 8) | (ip.f2 << 16) | (ip.f3 << 24)},
    };
    return;
  }
  case SOCKETS_IP_SOCKET_ADDRESS_IPV6: {
    if (output->tag != OUTPUT_SOCKADDR_V6) {
      abort();
    }

    sockets_ipv6_socket_address_t input_v6 = input.val.ipv6;
    sockets_ipv6_address_t ip = input_v6.address;

    *output->v6.addrlen = sizeof(struct sockaddr_in6);
    *output->v6.addr = (struct sockaddr_in6){
        .sin6_family = AF_INET6,
        .sin6_port = htons(input_v6.port),
        .sin6_addr = {.s6_addr =
                          {
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
                          }},
        // TODO wasi-sockets: do these need to be endian-reversed?
        .sin6_flowinfo = input_v6.flow_info,
        .sin6_scope_id = input_v6.scope_id,
    };
    return;
  }
  default: /* unreachable */
    abort();
  }
}

int __wasi_sockets_utils__posix_family(
    sockets_ip_address_family_t wasi_family) {
  switch (wasi_family) {
  case SOCKETS_IP_ADDRESS_FAMILY_IPV4:
    return AF_INET;
  case SOCKETS_IP_ADDRESS_FAMILY_IPV6:
    return AF_INET6;
  default: /* unreachable */
    abort();
  }
}

sockets_ip_socket_address_t
__wasi_sockets_utils__any_addr(sockets_ip_address_family_t family) {
  switch (family) {
  case SOCKETS_IP_ADDRESS_FAMILY_IPV4:
    return (sockets_ip_socket_address_t){.tag = SOCKETS_IP_SOCKET_ADDRESS_IPV4,
                                         .val = {.ipv4 = {
                                                     .port = 0,
                                                     .address = {0, 0, 0, 0},
                                                 }}};
  case SOCKETS_IP_ADDRESS_FAMILY_IPV6:
    return (sockets_ip_socket_address_t){
        .tag = SOCKETS_IP_SOCKET_ADDRESS_IPV6,
        .val = {.ipv6 = {
                    .port = 0,
                    .address = {0, 0, 0, 0, 0, 0, 0, 0},
                    .flow_info = 0,
                    .scope_id = 0,
                }}};
  default: /* unreachable */
    abort();
  }
}

int __wasi_sockets_utils__parse_port(const char *restrict port_str) {
  char *end = NULL;
  errno = 0;
  long port = strtol(port_str, &end, 10);

  // Check for various possible errors:
  //   - the input is not a valid number
  //   - the entire input string is not consumed
  //   - the number is not within the valid port range
  //   - the input does not start with a digit (strtol allows leading
  //     whitespace and optional sign)
  if (errno != 0 || end == NULL || *end != '\0' || port < 0 || port > 65535 ||
      !isdigit(*port_str)) {
    return -1;
  }

  return (int)port;
}

const service_entry_t *
__wasi_sockets_utils__get_service_entry_by_name(const char *name) {
  if (!name) {
    return NULL;
  }

  const service_entry_t *entry = __wasi_sockets_services_db;
  while (entry->s_name) {
    if (strcmp(name, entry->s_name) == 0) {
      return entry;
    }
    ++entry;
  }

  return NULL;
}

const service_entry_t *
__wasi_sockets_utils__get_service_entry_by_port(const uint16_t port) {
  const service_entry_t *entry = __wasi_sockets_services_db;
  while (entry->s_name) {
    if (entry->port == port) {
      return entry;
    }
    ++entry;
  }

  return NULL;
}
