#include "test.h"
#include <pthread.h>
#include <stdint.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

#define TLS_INIT 0x1234
#define TLS_CTOR 0x5678

static _Thread_local int tls_value = TLS_INIT;

__attribute__((constructor)) static void record_ctor_tls(void) {
  tls_value = TLS_CTOR;
}

static pthread_key_t key;

#define TASK_TSD ((void *)0x99)
#define THREAD_TSD ((void *)0x77)

static void key_dtor(void *arg) { (void)arg; }

static void *thread_main(void *arg) {
  (void)arg;

  if (tls_value != TLS_INIT)
    t_error("thread: tls_value = %#x, want %#x\n", tls_value, TLS_INIT);
  if (pthread_getspecific(key) != NULL)
    t_error("thread: inherited thread-specific data %p\n",
            pthread_getspecific(key));
  if (pthread_setspecific(key, THREAD_TSD) != 0)
    t_error("thread: pthread_setspecific failed\n");

  return NULL;
}

int main(void) {
  if (tls_value != TLS_CTOR)
    t_error("tls_value = %#x, want %#x\n", tls_value, TLS_CTOR);

  TEST(pthread_key_create(&key, key_dtor) == 0);
  TEST(pthread_getspecific(key) == NULL);
  TEST(pthread_setspecific(key, TASK_TSD) == 0);
  TEST(pthread_getspecific(key) == TASK_TSD);

  pthread_t t;
  if (pthread_create(&t, NULL, thread_main, NULL) == 0) {
    TEST(pthread_join(t, NULL) == 0);
    if (tls_value != TLS_CTOR)
      t_error("thread clobbered the task's tls_value: %#x\n", tls_value);
    if (pthread_getspecific(key) != TASK_TSD)
      t_error("thread clobbered the task's thread-specific data: %p\n",
              pthread_getspecific(key));
  } else {
#ifdef __wasi_cooperative_threads__
    t_error("pthread_create failed\n");
#endif
  }

  return t_status;
}
