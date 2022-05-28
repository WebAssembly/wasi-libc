#ifndef __wasilibc___struct_group_source_req_h
#define __wasilibc___struct_group_source_req_h

#include <__struct_sockaddr_storage.h>

struct group_source_req {
	uint32_t gsr_interface;
	struct sockaddr_storage gsr_group;
	struct sockaddr_storage gsr_source;
};

#endif
