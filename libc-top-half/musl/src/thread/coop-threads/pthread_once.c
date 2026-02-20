#include "pthread_impl.h"
enum {
    PTHREAD_ONCE_STATE_NOT_RUN = 0,
    PTHREAD_ONCE_STATE_RUNNING = 1,
    PTHREAD_ONCE_STATE_COMPLETED = 2
}

static struct {
	pthread_once_t *control;
	struct __waitlist_node *waiters;
} once_state;

int __pthread_once(pthread_once_t *control, void (*init)(void))
{
	/* Fast path: already completed */
	if (*control == PTHREAD_ONCE_STATE_COMPLETED) {
		return 0;
	}
	
	/* Try to become the initializer */
	if (*control == PTHREAD_ONCE_STATE_NOT_RUN) {
		*control = PTHREAD_ONCE_STATE_RUNNING;
		once_state.control = control;
		
		/* init may do something that blocks, in which case other threads
		 * may try to run pthread_once on the same control. They will
		 * see state PTHREAD_ONCE_STATE_RUNNING and wait on our waitlist. */
		init();
		
		*control = PTHREAD_ONCE_STATE_COMPLETED;
		__waitlist_wake_all(&once_state.waiters);
		return 0;
	}
	
	/* Another thread is initializing, wait for completion */
	while (*control == PTHREAD_ONCE_STATE_RUNNING) {
		__waitlist_wait_on(&once_state.waiters);
	}
	
	return 0;
}

weak_alias(__pthread_once, pthread_once);
