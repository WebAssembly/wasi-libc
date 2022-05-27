#ifndef __wasilibc___struct_if_addrs_h
#define __wasilibc___struct_if_addrs_h

#define __need_STDDEF_H_misc
#include <stddef.h>

#include <__struct_sockaddr.h>

struct if_addrs {
	struct if_addrs *ifa_next;
	char *ifa_name;
	unsigned ifa_flags;
	struct sockaddr *ifa_addr;
	struct sockaddr *ifa_netmask;
	union {
		struct sockaddr *ifu_broadaddr;
		struct sockaddr *ifu_dstaddr;
	} ifa_ifu;
	void *ifa_data;
};

#endif
