#ifndef __wasilibc___struct_ip_opts_h
#define __wasilibc___struct_ip_opts_h

#include <__struct_in_addr.h>

struct ip_opts {
	struct in_addr ip_dst;
	char ip_opts[40];
};

#endif
