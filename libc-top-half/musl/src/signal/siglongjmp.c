#include <setjmp.h>
#include <signal.h>
#include <setjmp.h>
#ifdef __wasilibc_unmodified_upstream
#include "syscall.h"
#endif
#include "pthread_impl.h"

_Noreturn void siglongjmp(sigjmp_buf buf, int ret)
{
	longjmp(buf, ret);
}