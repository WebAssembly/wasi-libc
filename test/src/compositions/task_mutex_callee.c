#include "test.h"
#include <callee.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static pthread_mutex_t normal = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t errorcheck;
static pthread_mutex_t recursive;
static bool initialized = false;
static bool released = false;

static void init(void) {
  if (initialized)
    return;
  pthread_mutexattr_t attr;
  TEST(pthread_mutexattr_init(&attr) == 0);
  TEST(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK) == 0);
  TEST(pthread_mutex_init(&errorcheck, &attr) == 0);
  TEST(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0);
  TEST(pthread_mutex_init(&recursive, &attr) == 0);
  TEST(pthread_mutexattr_destroy(&attr) == 0);
  initialized = true;
}

callee_callback_code_t exports_test_simple_i_hold(void) {
  init();
  TEST(pthread_mutex_lock(&normal) == 0);
  TEST(pthread_mutex_lock(&errorcheck) == 0);
  TEST(pthread_mutex_lock(&recursive) == 0);
  flockfile(stdout);
  return CALLEE_CALLBACK_CODE_YIELD;
}

callee_callback_code_t
exports_test_simple_i_hold_callback(callee_event_t *event) {
  TEST(event->event == CALLEE_EVENT_NONE);
  if (!released)
    return CALLEE_CALLBACK_CODE_YIELD;

  TEST(pthread_mutex_unlock(&normal) == 0);
  TEST(pthread_mutex_unlock(&errorcheck) == 0);
  TEST(pthread_mutex_unlock(&recursive) == 0);
  funlockfile(stdout);

  exports_test_simple_i_hold_return();
  return CALLEE_CALLBACK_CODE_EXIT;
}

void exports_test_simple_i_release(void) { released = true; }

// Runs while `hold` has everything locked, and this task is the same libc
// thread as `hold`.
void exports_test_simple_i_barge_in(void) {
  TEST(pthread_mutex_trylock(&normal) == EBUSY);
  TEST(pthread_mutex_lock(&errorcheck) == EDEADLK);
  TEST(pthread_mutex_trylock(&recursive) == 0);
  TEST(pthread_mutex_unlock(&recursive) == 0);
  TEST(ftrylockfile(stdout) == 0);
  funlockfile(stdout);
}

static bool spawn_released = false;

callee_callback_code_t exports_test_simple_i_spawn_later(void) {
  return CALLEE_CALLBACK_CODE_YIELD;
}

static void *try_recursive(void *arg) {
  (void)arg;
  // This is a different libc thread than the main thread, so the lock is
  // not owned by this thread.
  TEST(pthread_mutex_trylock(&recursive) == EBUSY);
  return NULL;
}

callee_callback_code_t
exports_test_simple_i_spawn_later_callback(callee_event_t *event) {
  TEST(event->event == CALLEE_EVENT_NONE);
  if (!spawn_released)
    return CALLEE_CALLBACK_CODE_YIELD;

  pthread_t t;
  if (pthread_create(&t, NULL, try_recursive, NULL) == 0) {
    TEST(pthread_join(t, NULL) == 0);
  }
  TEST(pthread_mutex_unlock(&recursive) == 0);

  exports_test_simple_i_spawn_later_return();
  return CALLEE_CALLBACK_CODE_EXIT;
}

// Leaves `recursive` locked by the main thread.
void exports_test_simple_i_lock(void) {
  init();
  TEST(pthread_mutex_lock(&recursive) == 0);
}

void exports_test_simple_i_release_spawn(void) { spawn_released = true; }

uint32_t exports_test_simple_i_t_status(void) { return t_status; }
