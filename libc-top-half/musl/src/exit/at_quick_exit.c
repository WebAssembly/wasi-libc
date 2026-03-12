#include <stdlib.h>
#include "libc.h"
#include "lock.h"
#include "fork_impl.h"

#define COUNT 32

static void (*funcs[COUNT])(void);
static int count;
DECLARE_STRONG_LOCK(lock, static);

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
