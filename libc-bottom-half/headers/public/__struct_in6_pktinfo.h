#ifndef __wasilibc___struct_in6_pktinfo_h
#define __wasilibc___struct_in6_pktinfo_h

#include <__struct_in6_addr.h>

struct in6_pktinfo {
	struct in6_addr ipi6_addr;
	unsigned ipi6_ifindex;
};

#endif
