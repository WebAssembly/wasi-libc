#ifndef __wasilibc___struct_cmsghdr_h
#define __wasilibc___struct_cmsghdr_h

#define __need_STDDEF_H_misc
#include <stddef.h>

#include <__typedef_socklen_t.h>

struct cmsghdr {
    socklen_t cmsg_len;
    int cmsg_level;
    int cmsg_type;
    _Alignas(max_align_t) unsigned char __cmsg_data[];
};

#endif
