#ifndef __wasilibc___struct_group_req_h
#define __wasilibc___struct_group_req_h

#include <__struct_sockaddr_storage.h>

struct group_req {
	uint32_t gr_interface;
	struct sockaddr_storage gr_group;
};

#endif
