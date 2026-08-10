#include "shared/tls_a.h"
#include "shared/tls_b.h"
#include "shared/tls_none.h"
#include "test.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wasi/version.h>

_Thread_local int main_counter = 0x0c0c0c0c;
_Thread_local char main_name[16] = "main";

static int overlaps(void *a, size_t a_size, void *b, size_t b_size) {
  uintptr_t a_start = (uintptr_t)a;
  uintptr_t b_start = (uintptr_t)b;
  return a_start < b_start + b_size && b_start < a_start + a_size;
}

#define THREADS 4

static pthread_barrier_t barrier;

struct thread_args {
  int index;
  void *a_addr;
  void *b_addr;
  void *main_addr;
};

static void assert_initial_tls() {
  if (a_counter_get() != A_COUNTER_INIT)
    t_error("a_counter = %#x, want %#x\n", a_counter_get(), A_COUNTER_INIT);
  if (strcmp(a_name_get(), A_NAME_INIT) != 0)
    t_error("a_name = \"%s\", want \"%s\"\n", a_name_get(), A_NAME_INIT);
  if (a_zeroed_sum() != 0)
    t_error("a_zeroed_sum = %d, want 0\n", a_zeroed_sum());
  if (b_counter_get() != B_COUNTER_INIT)
    t_error("b_counter = %#llx, want %#llx\n", b_counter_get(),
            (long long)B_COUNTER_INIT);
  if (main_counter != 0x0c0c0c0c)
    t_error("main_counter = %#x, want 0x0c0c0c0c\n", main_counter);
  for (size_t i = 0; i < B_BLOB_SIZE; i++) {
    if (b_blob_get(i) != 0)
      t_error("b_blob[%zu] = %d, want 0\n", i, b_blob_get(i));
  }

  // Alignment has to hold for every thread's block, not just the main one.
  if ((uintptr_t)b_aligned_addr() % B_ALIGN != 0)
    t_error("b_aligned is at %p, want %d-byte alignment\n", b_aligned_addr(), B_ALIGN);

  // Each library's thread-locals must live in disjoint memory.
  void *a_addr = a_counter_addr();
  void *b_addr = b_counter_addr();
  void *main_addr = &main_counter;
  if (overlaps(a_addr, sizeof(int), b_addr, sizeof(long long)))
    t_error("libtls_a and libtls_b share TLS (%p vs %p)\n", a_addr, b_addr);
  if (overlaps(a_addr, sizeof(int), main_addr, sizeof(int)))
    t_error("libtls_a and the program share TLS (%p vs %p)\n", a_addr,
            main_addr);
  if (overlaps(b_addr, sizeof(long long), main_addr, sizeof(int)))
    t_error("libtls_b and the program share TLS (%p vs %p)\n", b_addr,
            main_addr);
}

static void clobber_tls(int index) {
  int value = 0x1000 + index;
  a_counter_set(value);
  b_counter_set(value + 1);
  main_counter = value + 2;
  a_zeroed_fill(index + 1);
  for (size_t i = 0; i < B_BLOB_SIZE; i++)
    b_blob_set(i, value + (int)i);

  char name[A_NAME_SIZE];
  snprintf(name, sizeof(name), "thread-%d", index);
  a_name_set(name);
}

static void assert_clobbered_tls(int index) {
  int value = 0x1000 + index;
  char name[A_NAME_SIZE];
  snprintf(name, sizeof(name), "thread-%d", index);
  if (a_counter_get() != value)
    t_error("thread %d: a_counter = %#x, want %#x\n", index,
            a_counter_get(), value);
  if (b_counter_get() != value + 1)
    t_error("thread %d: b_counter = %#llx, want %#x\n", index,
            b_counter_get(), value + 1);
  if (main_counter != value + 2)
    t_error("thread %d: main_counter = %#x, want %#x\n", index,
            main_counter, value + 2);
  if (strcmp(a_name_get(), name) != 0)
    t_error("thread %d: a_name = \"%s\", want \"%s\"\n", index,
            a_name_get(), name);
  if (a_zeroed_sum() != (index + 1) * 8)
    t_error("thread %d: a_zeroed_sum = %d, want %d\n", index,
            a_zeroed_sum(), (index + 1) * 8);
  for (size_t i = 0; i < B_BLOB_SIZE; i++) {
    if (b_blob_get(i) != value + (int)i)
      t_error("thread %d: b_blob[%zu] = %#x, want %#x\n", index, i,
              b_blob_get(i), value + (int)i);
  }
  if (b_read_a_counter() != value)
    t_error("thread %d: b_read_a_counter = %#x, want %#x\n", index,
            b_read_a_counter(), value);
}

static void *thread_main(void *raw) {
  struct thread_args *args = raw;

  assert_initial_tls();

  args->a_addr = a_counter_addr();
  args->b_addr = b_counter_addr();
  args->main_addr = &main_counter;

  pthread_barrier_wait(&barrier);
  clobber_tls(args->index);

  pthread_barrier_wait(&barrier);
  assert_clobbered_tls(args->index);

  return NULL;
}

static void test_threads(void) {
  a_counter_set(0xdead);
  b_counter_set(0xbeef);
  main_counter = 0xf00d;
  a_name_set("main-thread");
  a_zeroed_fill(99);
  for (size_t i = 0; i < B_BLOB_SIZE; i++)
    b_blob_set(i, 0x7777);

  int err = pthread_barrier_init(&barrier, NULL, THREADS);
  if (err != 0)
    t_error("pthread_barrier_init failed: %d\n", err);

  pthread_t threads[THREADS];
  struct thread_args args[THREADS] = {0};
  for (int i = 0; i < THREADS; i++) {
    args[i].index = i;
    err = pthread_create(&threads[i], NULL, thread_main, &args[i]);
    if (err != 0) {
#ifdef __wasi_cooperative_threads__
      t_error("pthread_create(%d) failed: %d\n", i, err);
#else
      return;
#endif
    }
  }
  for (int i = 0; i < THREADS; i++) {
    err = pthread_join(threads[i], NULL);
    if (err != 0)
      t_error("pthread_join(%d) failed: %d\n", i, err);
  }
  pthread_barrier_destroy(&barrier);

  // No two threads, and no thread and the main thread, may share a block.
  for (int i = 0; i < THREADS; i++) {
    if (args[i].a_addr == a_counter_addr())
      t_error("thread %d shares libtls_a TLS with the main thread (%p)\n", i,
              args[i].a_addr);
    if (args[i].b_addr == b_counter_addr())
      t_error("thread %d shares libtls_b TLS with the main thread (%p)\n", i,
              args[i].b_addr);
    if (args[i].main_addr == (void *)&main_counter)
      t_error("thread %d shares program TLS with the main thread (%p)\n", i,
              args[i].main_addr);
    for (int j = i + 1; j < THREADS; j++) {
      if (args[i].a_addr == args[j].a_addr)
        t_error("threads %d and %d share libtls_a TLS (%p)\n", i, j,
                args[i].a_addr);
      if (args[i].b_addr == args[j].b_addr)
        t_error("threads %d and %d share libtls_b TLS (%p)\n", i, j,
                args[i].b_addr);
      if (args[i].main_addr == args[j].main_addr)
        t_error("threads %d and %d share program TLS (%p)\n", i, j,
                args[i].main_addr);
    }
  }

  // Finally, the main thread's own TLS must have survived all of that.
  if (a_counter_get() != 0xdead)
    t_error("main a_counter = %#x, want 0xdead\n", a_counter_get());
  if (b_counter_get() != 0xbeef)
    t_error("main b_counter = %#llx, want 0xbeef\n", b_counter_get());
  if (main_counter != 0xf00d)
    t_error("main main_counter = %#x, want 0xf00d\n", main_counter);
  if (strcmp(a_name_get(), "main-thread") != 0)
    t_error("main a_name = \"%s\", want \"main-thread\"\n", a_name_get());
  if (a_zeroed_sum() != 99 * 8)
    t_error("main a_zeroed_sum = %d, want %d\n", a_zeroed_sum(), 99 * 8);
  for (size_t i = 0; i < B_BLOB_SIZE; i++) {
    if (b_blob_get(i) != 0x7777)
      t_error("main b_blob[%zu] = %#x, want 0x7777\n", i, b_blob_get(i));
  }
}

int main(void) {
  assert_initial_tls();
  clobber_tls(0);
  assert_clobbered_tls(0);

  if (none_add(2, 3) != 5)
    t_error("none_add(2, 3) = %d, want 5\n", none_add(2, 3));
  none_state_set(7);
  if (none_state_get() != 7)
    t_error("none_state_get = %d, want 7\n", none_state_get());

  test_threads();

  return t_status;
}
