#include <stdlib.h>
#include "libc.h"
#include "lock.h"
#include "fork_impl.h"

#define COUNT 32

static void (*funcs[COUNT])(void);
static int count;
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
// All locks here can be weak, because the locking is only needed to protect against
// concurrent manipulation of the handler table, which hits no context switch points.
static __lock_t lock[1];
__lock_t *const __at_quick_exit_lockptr = lock;
#endif

void __funcs_on_quick_exit()
{
	void (*func)(void);
	WEAK_LOCK(lock);
	while (count > 0) {
		func = funcs[--count];
		WEAK_UNLOCK(lock);
		func();
		WEAK_LOCK(lock);
	}
}

int at_quick_exit(void (*func)(void))
{
	int r = 0;
	WEAK_LOCK(lock);
	if (count == 32) r = -1;
	else funcs[count++] = func;
	WEAK_UNLOCK(lock);
	return r;
}
