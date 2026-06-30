#include "pthread_impl.h"
#include <threads.h>

static int __pthread_detach(pthread_t t) {
  int state = t->detach_state;

  // Detaching from an already-detached thread is an error
  if (state == DT_DETACHED) {
    return EINVAL;
  }

  if (state == DT_EXITING || state == DT_EXITED) {
    __pthread_join(t, 0);
  }

  t->detach_state = DT_DETACHED;
  return 0;
}

weak_alias(__pthread_detach, pthread_detach);
weak_alias(__pthread_detach, thrd_detach);
