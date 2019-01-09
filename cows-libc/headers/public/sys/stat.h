#ifndef __cowslibc_sys_stat_h
#define __cowslibc_sys_stat_h

#include <__struct_stat.h>

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

#define S_IFMT \
    (S_IFBLK | S_IFCHR | S_IFDIR | S_IFIFO | S_IFLNK | S_IFREG | S_IFSOCK)
#define S_IFBLK 0x6000
#define S_IFCHR 0x2000
#define S_IFDIR 0x4000
#define S_IFLNK 0xa000
#define S_IFREG 0x8000
#define S_IFSOCK 0xc000
#define S_IFIFO 0xc000

#define S_ISBLK(m) (((m)&S_IFMT) == S_IFBLK)
#define S_ISCHR(m) (((m)&S_IFMT) == S_IFCHR)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((m)&S_IFMT) == S_IFIFO)
#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISSOCK(m) (((m)&S_IFMT) == S_IFSOCK)

#define UTIME_NOW (-1)
#define UTIME_OMIT (-2)

#endif
