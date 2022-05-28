#ifndef __wasilibc___struct_group_filter_h
#define __wasilibc___struct_group_filter_h

#include <__struct_sockaddr_storage.h>

struct group_filter {
	uint32_t gf_interface;
	struct sockaddr_storage gf_group;
	uint32_t gf_fmode;
	uint32_t gf_numsrc;
	struct sockaddr_storage gf_slist[1];
};

#endif
