#include "lock.h"
#include "pthread_impl.h"
#include <assert.h>
#include <wasi/api.h>

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

void __lock(volatile int *lock) {
  int tid = wasip3_thread_index();
  if (*lock == tid) {
    /* Trap on recursive locking. */
    __builtin_trap();
  }

  /* Loop until we acquire the lock. */
  while (*lock != 0) {
    __futexwait(lock, *lock, 0);
    /* After waking, the lock might still be held by another
     * thread that was scheduled before us, so loop back. */
  }

  *lock = tid;
}

void __unlock(volatile int *lock) {
  int tid = wasip3_thread_index();
  if (*lock != tid) {
    /* We're trying to unlock a lock we don't own. */
    __builtin_trap();
  }

  *lock = 0;
  /* Awake one waiter; the others will be resumed on future unlocks. */
  __wake(lock, 1, 1);
}
