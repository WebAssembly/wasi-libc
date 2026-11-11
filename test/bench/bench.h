#ifndef WASI_LIBC_BENCH_H
#define WASI_LIBC_BENCH_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

typedef struct {
  size_t n;       // Iterations BENCH_LOOP runs.
  intptr_t arg;   // Case argument passed to bench_run.
  uint64_t bytes; // Bytes processed per iteration, set to report MB/s.
  uint64_t t0;    // Start timestamp (ns), set by bench_start.
  uint64_t ns;    // Elapsed time (ns), set by bench_stop.
} bench_t;

typedef struct {
  const char *filter;
  long count;
  long ms;
} bench_runner_t;

// Parses CLI flags (--filter, --count, --ms, --help) or exits on invalid args.
bench_runner_t bench_init(int argc, char **argv);

// Runs fn(b) with b->arg = arg when name matches r->filter.
void bench_run(const bench_runner_t *r, const char *name, void (*fn)(bench_t *),
               intptr_t arg);

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

// Evaluates x and assumes reachable memory changed. Pass input pointers before
// BENCH_LOOP and results inside it so calls are not hoisted or dropped.
#define BENCH_KEEP(x) __asm__ volatile("" : : "r"(x) : "memory")

// Runs the loop body b->n times and records elapsed time. Local i_ avoids
// reloading b->n after BENCH_KEEP, and || stops the timer once i_ reaches 0.
#define BENCH_LOOP(b)                                                          \
  for (size_t i_ = bench_start(b); i_ > 0 || (bench_stop(b), 0); i_--)

#endif
