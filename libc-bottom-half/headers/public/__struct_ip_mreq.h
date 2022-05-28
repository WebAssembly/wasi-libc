#ifndef __wasilibc___struct_ip_mreq_h
#define __wasilibc___struct_ip_mreq_h

#include <__struct_in_addr.h>

struct ip_mreq {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

#endif
