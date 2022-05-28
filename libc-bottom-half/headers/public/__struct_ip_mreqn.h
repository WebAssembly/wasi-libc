#ifndef __wasilibc___struct_ip_mreqn_h
#define __wasilibc___struct_ip_mreqn_h

#include <__struct_in_addr.h>

struct ip_mreqn {
	struct in_addr imr_multiaddr;
	struct in_addr imr_address;
	int imr_ifindex;
};

#endif
