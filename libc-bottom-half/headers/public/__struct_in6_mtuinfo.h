#ifndef __wasilibc___struct_in6_mtuinfo_h
#define __wasilibc___struct_in6_mtuinfo_h

#include <__struct_sockaddr_in6.h>

struct ip6_mtuinfo {
	struct sockaddr_in6 ip6m_addr;
	uint32_t ip6m_mtu;
};

#endif
