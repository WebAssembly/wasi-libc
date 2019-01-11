#ifndef	_ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#include <bits/errno.h>

#ifdef __wasm_musl_unmodified_upstream__
#ifdef __GNUC__
__attribute__((const))
#endif
int *__errno_location(void);
#define errno (*__errno_location())
#else
#include <__errno.h>
#endif

#ifdef _GNU_SOURCE
extern char *program_invocation_short_name, *program_invocation_name;
#endif

#ifdef __cplusplus
}
#endif

#endif

