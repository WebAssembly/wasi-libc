#ifndef WASI_LIBC_BENCH_H
#define WASI_LIBC_BENCH_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  size_t n;
  intptr_t arg;
  uint64_t t0;
  uint64_t ns;
  uint64_t bytes;
} bench_t;

typedef struct bench_entry {
  const char *name;
  void (*fn)(bench_t *);
  intptr_t arg;
  struct bench_entry *next;
} bench_entry_t;

static bench_entry_t *bench_head, **bench_tail = &bench_head;

static inline void bench_register(bench_entry_t *e) {
  *bench_tail = e;
  bench_tail = &e->next;
}

static inline uint64_t bench_now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

static inline size_t bench_start(bench_t *b) {
  b->t0 = bench_now_ns();
  return b->n;
}

static inline void bench_stop(bench_t *b) { b->ns = bench_now_ns() - b->t0; }

static inline void bench_bytes(bench_t *b, uint64_t n) { b->bytes = n; }

// Prevent constant folding of inputs or hoisting pure calls out of BENCH_LOOP.
static inline void bench_clobber(const void *p) {
  __asm__ volatile("" : : "r"(p) : "memory");
}

#define BENCH_DO_NOT_OPTIMIZE(x)                                               \
  do {                                                                         \
    __typeof__(x) _v = (x);                                                    \
    __asm__ volatile("" : "+r"(_v) : : "memory");                              \
  } while (0)

// Keep the countdown in a local variable so memory clobbers do not spill it.
#define BENCH_LOOP(b)                                                          \
  for (size_t _i = bench_start(b); _i > 0 || (bench_stop(b), 0); _i--)

#define BENCH(suite, fn_name)                                                  \
  static void bench_fn_##suite##_##fn_name(bench_t *);                         \
  __attribute__((constructor)) static void bench_reg_##suite##_##fn_name(      \
      void) {                                                                  \
    static bench_entry_t e = {#suite "/" #fn_name,                             \
                              bench_fn_##suite##_##fn_name, 0, NULL};          \
    bench_register(&e);                                                        \
  }                                                                            \
  static void bench_fn_##suite##_##fn_name(bench_t *b)

#define BENCH_ARG(suite, fn, val)                                              \
  __attribute__((constructor)) static void bench_reg_##suite##_##fn##_##val(   \
      void) {                                                                  \
    static bench_entry_t e = {#suite "/" #fn "/" #val, fn, (val), NULL};       \
    bench_register(&e);                                                        \
  }                                                                            \
  struct bench_semi_##suite##_##fn##_##val

static inline int bench_main(int argc, char **argv) {
  const char *filter = NULL;
  int count = 1;
  uint64_t target_ns = 100000000ull;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-filter") && i + 1 < argc)
      filter = argv[++i];
    else if (!strcmp(argv[i], "-count") && i + 1 < argc)
      count = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-ms") && i + 1 < argc)
      target_ns = (uint64_t)atoi(argv[++i]) * 1000000ull;
  }
  const size_t max_n = 1000000000u;
  for (bench_entry_t *e = bench_head; e; e = e->next) {
    if (filter && !strstr(e->name, filter))
      continue;
    for (int c = 0; c < count; c++) {
      size_t n = 1;
      bench_t b;
      for (;;) {
        b = (bench_t){.n = n, .arg = e->arg};
        e->fn(&b);
        if (b.ns >= target_ns || n >= max_n)
          break;
        uint64_t next = (b.ns == 0 || b.ns < target_ns / 10)
                            ? (uint64_t)n * 10
                            : (uint64_t)n * target_ns / b.ns + n / 5;
        if (next <= n)
          next = (uint64_t)n + 1;
        n = next > max_n ? max_n : (size_t)next;
      }
      double ns_op = (double)b.ns / (double)n;
      printf("Benchmark_%-24s\t%10zu\t%10.2f ns/op", e->name, n, ns_op);
      if (b.bytes && ns_op > 0)
        printf("\t%10.2f MB/s", ((double)b.bytes * 1e3) / ns_op);
      putchar('\n');
      fflush(stdout);
    }
  }
  return 0;
}

#endif
