#ifndef __wasilibc___struct_ipv6_mreq_h
#define __wasilibc___struct_ipv6_mreq_h

#include <__struct_in6_addr.h>

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned        ipv6mr_interface;
};

#endif
