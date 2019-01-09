#ifndef __cowslibc___struct_iovec_h
#define __cowslibc___struct_iovec_h

#define __need_size_t
#include <stddef.h>

struct iovec {
    void *iov_base;
    size_t iov_len;
};

#endif
