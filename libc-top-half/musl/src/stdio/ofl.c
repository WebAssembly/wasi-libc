#include "stdio_impl.h"
#include "lock.h"
#include "fork_impl.h"

static FILE *ofl_head;
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
static __lock_t ofl_lock[1];
__lock_t *const __stdio_ofl_lockptr = ofl_lock;
#endif

FILE **__ofl_lock()
{
	STRONG_LOCK(ofl_lock);
	return &ofl_head;
}

void __ofl_unlock()
{
	STRONG_UNLOCK(ofl_lock);
}
