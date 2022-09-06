#include <stdlib.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#else
#include <wasi/api.h>
#endif

_Noreturn void _Exit(int ec)
{
#ifdef __wasilibc_unmodified_upstream
	__syscall(SYS_exit_group, ec);
	for (;;) __syscall(SYS_exit, ec);
#else
	for (;;) __wasi_proc_exit(ec);
#endif
}

weak_alias(_Exit, _exit);