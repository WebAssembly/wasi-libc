#ifndef __wasm_sysroot_sys_types_h
#define __wasm_sysroot_sys_types_h

#define __need_size_t
#include <stddef.h>

#include <__typedef_clock_t.h>
#include <__typedef_time_t.h>
#include <__typedef_blksize_t.h>
#include <__typedef_off_t.h>
#include <__typedef_ssize_t.h>
#include <__typedef_suseconds_t.h>
#include <__typedef_nlink_t.h>

/* Define these as 64-bit signed integers to support files larger than 2 GiB. */
typedef long long blksize_t;
typedef long long off_t;

/* This is defined to be the same size as size_t. */
typedef long ssize_t;

/* Define this to be 64-bit as its main use is in struct timeval where the
   extra space would otherwise be padding. */
typedef long long suseconds_t;

/* Follow x32 in defining this as 64-bit. */
typedef long long nlink_t;

#endif
