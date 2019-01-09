#ifndef __wasm_basics_sys_stat_h
#define __wasm_basics_sys_stat_h

#include <__struct_stat.h>

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

#endif
