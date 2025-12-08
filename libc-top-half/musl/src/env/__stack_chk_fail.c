#include <string.h>
#include <stdint.h>
#include <unistd.h>
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
#include "pthread_impl.h"
#else
// In non-_REENTRANT, include it for `a_crash`
# include "atomic.h"
#endif

uintptr_t __stack_chk_guard;

void __stack_chk_fail(void)
{
	a_crash();
}

hidden void __stack_chk_fail_local(void);

weak_alias(__stack_chk_fail, __stack_chk_fail_local);

#ifndef __wasilibc_unmodified_upstream
#ifdef __wasilibc_use_wasip2
# include <wasi/libc.h>
#else
# include <wasi/api.h>
#endif

extern uintptr_t __stack_pointer;

__attribute__((constructor(60)))
static void __wasilibc_init_ssp(void) {

	uintptr_t entropy;
#ifdef __wasilibc_use_wasip2
        int len = sizeof(uintptr_t);

        int r = __wasilibc_random(&entropy, len);
#else
	int r = __wasi_random_get((uint8_t *)&entropy, sizeof(uintptr_t));
	if (r != 0) _exit(1);

	__asm__ volatile (
		".globaltype __stack_chk_guard, i32\n"
		"local.get 0\n"
		"i32.load 12\n"
		"global.set __stack_chk_guard\n"
		"local.get 0\n"
		"i32.const 0\n"
		"i32.store 12\n"
	);
}
#endif
