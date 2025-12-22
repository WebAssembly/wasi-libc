#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <wasi/libc-busywait.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main() {
  struct timespec ts;
  int ret;

  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 1;

  __wasilibc_enable_futex_busywait_on_current_thread();

  pthread_mutex_lock(&mutex);

  ret = pthread_cond_timedwait(&cond, &mutex, &ts);

  assert(ret == ETIMEDOUT);

  pthread_mutex_unlock(&mutex);
  return 0;
}
