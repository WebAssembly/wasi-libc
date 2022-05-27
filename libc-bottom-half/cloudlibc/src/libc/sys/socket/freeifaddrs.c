#include <common/errno.h>
#include <common/net.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

#include <__struct_if_addrs.h>

void freeif_addrs(struct if_addrs *restrict ifa) {
  for (;ifa != NULL;) {
    if (ifa->ifa_addr != NULL) {
      free(ifa->ifa_addr);
      ifa->ifa_addr = NULL;
    }
    if (ifa->ifa_netmask != NULL) {
      free(ifa->ifa_netmask);
      ifa->ifa_netmask = NULL;
    }
    if (ifa->ifa_ifu.ifu_broadaddr != NULL) {
      free(ifa->ifa_ifu.ifu_broadaddr);
      ifa->ifa_ifu.ifu_broadaddr = NULL;
    }
    if (ifa->ifa_ifu.ifu_dstaddr != NULL) {
      free(ifa->ifa_ifu.ifu_dstaddr);
      ifa->ifa_ifu.ifu_dstaddr = NULL;
    }
    if (ifa->ifa_data != NULL) {
      free(ifa->ifa_data);
      ifa->ifa_data = NULL;
    }
    struct if_addrs * next = ifa->ifa_next;
    free(ifa);
    ifa = next;
  }
}
