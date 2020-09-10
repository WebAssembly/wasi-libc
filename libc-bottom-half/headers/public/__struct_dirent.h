#ifndef __wasilibc___struct_dirent_h
#define __wasilibc___struct_dirent_h

#include <__typedef_ino_t.h>

#define _DIRENT_HAVE_D_TYPE

struct dirent {
    /// Similar to the traditional `d_off` field, however not compatible with
    /// `seekdir`. Use with `__wasilibc_seekdir` instead.
    unsigned long long d_loc;
    ino_t d_ino;
    unsigned char d_type;
    char d_name[];
};

#endif
