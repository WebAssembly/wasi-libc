#ifndef __wasilibc___struct_ip_msfilter_h
#define __wasilibc___struct_ip_msfilter_h

#include <__struct_in_addr.h>

struct ip_msfilter {
	struct in_addr imsf_multiaddr;
	struct in_addr imsf_interface;
	uint32_t imsf_fmode;
	uint32_t imsf_numsrc;
	struct in_addr imsf_slist[1];
};

#endif
