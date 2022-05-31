#include <common/errno.h>
#include <common/net.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

#include <__struct_if_addrs.h>

void freeif_addrs(struct if_addrs *restrict ifa);

int getif_addrs(struct if_addrs **restrict ifap) {
  __wasi_size_t nips = 10;
  struct __wasi_addr_cidr_t *ips_heap = malloc(sizeof(struct __wasi_addr_cidr_t) * nips);
  if (ips_heap == NULL) {
    errno = ENOMEM;
    return -1;
  }
  memset(ips_heap, 0, sizeof(struct __wasi_addr_cidr_t) * nips);
  
  __wasi_errno_t error = __wasi_port_addr_list(ips_heap, &nips);
  if (error == EOVERFLOW) {
    free(ips_heap);
    ips_heap = malloc(sizeof(struct __wasi_addr_cidr_t) * nips);
    if (ips_heap == NULL) {
      errno = ENOMEM;
      return -1;
    }
    memset(ips_heap, 0, sizeof(struct __wasi_addr_cidr_t) * nips);

    // try again but with a bigger buffer (returned in nips)
    error = __wasi_port_addr_list(ips_heap, &nips);
  }
  if (error != 0) {
    free(ips_heap);
    errno = error;
    return -1;
  }

  struct if_addrs *last = NULL;
  *ifap = NULL;
  for (__wasi_size_t n = 0; n < nips; n++) {
    struct if_addrs *ifa = malloc(sizeof(struct if_addrs));
    if (ifa == NULL) {
      freeif_addrs(*ifap);
      *ifap = NULL;
      free(ips_heap);
      errno = ENOMEM;
      return -1;
    }
    memset(ifa, 0, sizeof(struct if_addrs));

    struct __wasi_addr_cidr_t * ip = &ips_heap[n];
    if (ip->tag == __WASI_ADDRESS_FAMILY_INET4) {
      struct sockaddr_in *addr4 = malloc(sizeof(struct sockaddr_in));
      if (addr4 == NULL) {
        freeif_addrs(ifa);
        continue;
      }
      addr4->sin_family = AF_INET;
      addr4->sin_port = 0;
      addr4->sin_addr.s_addr = *((in_addr_t*)&ip->u.inet4.addr);
      ifa->ifa_addr = (struct sockaddr*)addr4;
    } else if (ip->tag == __WASI_ADDRESS_FAMILY_INET6) {
      struct sockaddr_in6 *addr6 = malloc(sizeof(struct sockaddr_in6));
      if (addr6 == NULL) {
        freeif_addrs(ifa);
        continue;
      }
      addr6->sin6_family = AF_INET6;
      addr6->sin6_flowinfo = 0;
      addr6->sin6_scope_id = 0;
      addr6->sin6_port = 0;
      memcpy(&addr6->sin6_addr.s6_addr, &ip->u.inet6.addr, sizeof(struct in6_addr));
      ifa->ifa_addr = (struct sockaddr*)addr6; 
    } else {
      freeif_addrs(ifa);
      continue;
    }

    if (last != NULL) {
      last->ifa_next = ifa;
    } else {
      *ifap = ifa;
    }
    last = ifa;
  }

  free(ips_heap);
  return 0;
}
