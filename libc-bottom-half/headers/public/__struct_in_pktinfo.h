#ifndef __wasilibc___struct_in_pktinfo_h
#define __wasilibc___struct_in_pktinfo_h

#include <__struct_in_addr.h>

struct in_pktinfo {
	int ipi_ifindex;
	struct in_addr ipi_spec_dst;
	struct in_addr ipi_addr;
};

#endif
