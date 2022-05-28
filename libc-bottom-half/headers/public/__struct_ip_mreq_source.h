#ifndef __wasilibc___struct_ip_mreq_source_h
#define __wasilibc___struct_ip_mreq_source_h

#include <__struct_in_addr.h>

struct ip_mreq_source {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
	struct in_addr imr_sourceaddr;
};

#endif
