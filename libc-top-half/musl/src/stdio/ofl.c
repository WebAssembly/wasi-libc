#include "stdio_impl.h"
#include "lock.h"
#include "fork_impl.h"

static FILE *ofl_head;
static DECLARE_STRONG_LOCK(ofl_lock);

FILE **__ofl_lock()
{
	STRONG_LOCK(ofl_lock);
	return &ofl_head;
}

void __ofl_unlock()
{
	STRONG_UNLOCK(ofl_lock);
}
