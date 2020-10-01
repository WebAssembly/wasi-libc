#ifndef __wasilibc___struct_dirent_h
#define __wasilibc___struct_dirent_h

#include <__typedef_ino_t.h>

#define _DIRENT_HAVE_D_TYPE
#define _DIRENT_HAVE_D_NAMLEN

struct dirent {
    /// Similar to the traditional `d_off` field, however not compatible with
    /// `seekdir`. Use with `__wasilibc_seekdir` instead.
    unsigned long long d_loc;

    /// The inode number of the entry.
    ino_t d_ino;

    /// The length of the string in `d_name`, not including the trailing NUL.
    unsigned d_namlen;

    /// Type of filesystem entry (file, directory, etc.).
    unsigned char d_type;

    // Padding so that the layout of `dirent` matches `__wasi_dirent_t`.
    unsigned char __padding[3];

    /// The name of the entry, with a trailing NUL.
    char d_name[];
};

#endif
