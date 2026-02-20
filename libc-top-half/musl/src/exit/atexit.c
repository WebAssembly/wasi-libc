#include <stdlib.h>
#include <stdint.h>
#include "libc.h"
#include "lock.h"
#include "fork_impl.h"

#define malloc __libc_malloc
#define calloc __libc_calloc
#define realloc undef
#define free undef

/* Ensure that at least 32 atexit handlers can be registered without malloc */
#define COUNT 32

static struct fl
{
	struct fl *next;
	void (*f[COUNT])(void *);
	void *a[COUNT];
} builtin, *head;

static int slot;

#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
#include "lock.h"

// All locks here can be weak, because the locking is only needed to protect against
// concurrent manipulation of the handler table, which hits no context switch points.
__lock_t lock[1];
__lock_t *const __atexit_lockptr = lock;
#endif

void __funcs_on_exit()
{
	void (*func)(void *), *arg;
	WEAK_LOCK(lock);
	for (; head; head=head->next, slot=COUNT) while(slot-->0) {
		func = head->f[slot];
		arg = head->a[slot];
		WEAK_UNLOCK(lock);
		func(arg);
		WEAK_LOCK(lock);
	}
}

void __cxa_finalize(void *dso)
{
}

int __cxa_atexit(void (*func)(void *), void *arg, void *dso)
{
	WEAK_LOCK(lock);

	/* Defer initialization of head so it can be in BSS */
	if (!head) head = &builtin;

	/* If the current function list is full, add a new one */
	if (slot==COUNT) {
		struct fl *new_fl = calloc(sizeof(struct fl), 1);
		if (!new_fl) {
			WEAK_UNLOCK(lock);
			return -1;
		}
		new_fl->next = head;
		head = new_fl;
		slot = 0;
	}

	/* Append function to the list. */
	head->f[slot] = func;
	head->a[slot] = arg;
	slot++;

	WEAK_UNLOCK(lock);
	return 0;
}

static void call(void *p)
{
	((void (*)(void))(uintptr_t)p)();
}

int atexit(void (*func)(void))
{
	return __cxa_atexit(call, (void *)(uintptr_t)func, 0);
}
