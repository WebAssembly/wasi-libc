#include <string.h>

#include "bench.h"

static void fill_pair(unsigned char *l, unsigned char *r, size_t n) {
  for (size_t i = 0; i < n + 8; i++)
    l[i] = r[i] = (unsigned char)(i * 7 + 1);
  // Ensure reads past n observe different bytes.
  l[n] = 1;
  r[n] = 2;
}

static void equal(bench_t *b) {
  size_t n = (size_t)b->arg;
  unsigned char buf_l[1040], buf_r[1040];
  unsigned char *l = buf_l + 1, *r = buf_r + 3;
  fill_pair(l, r, n);
  BENCH_KEEP(l);
  BENCH_KEEP(r);
  b->bytes = n;
  BENCH_LOOP(b) BENCH_KEEP(memcmp(l, r, n));
}

static void diff_first(bench_t *b) {
  size_t n = (size_t)b->arg;
  unsigned char buf_l[1040], buf_r[1040];
  unsigned char *l = buf_l + 1, *r = buf_r + 3;
  fill_pair(l, r, n);
  l[0] = 0x80;
  r[0] = 0x7f;
  BENCH_KEEP(l);
  BENCH_KEEP(r);
  BENCH_LOOP(b) BENCH_KEEP(memcmp(l, r, n));
}

static void diff_last(bench_t *b) {
  size_t n = (size_t)b->arg;
  unsigned char buf_l[1040], buf_r[1040];
  unsigned char *l = buf_l + 1, *r = buf_r + 3;
  fill_pair(l, r, n);
  l[n - 1] = 0x80;
  r[n - 1] = 0x7f;
  BENCH_KEEP(l);
  BENCH_KEEP(r);
  b->bytes = n;
  BENCH_LOOP(b) BENCH_KEEP(memcmp(l, r, n));
}

int main(int argc, char **argv) {
  bench_runner_t r = bench_init(argc, argv);
  bench_run(&r, "memcmp/equal/4", equal, 4);
  bench_run(&r, "memcmp/equal/8", equal, 8);
  bench_run(&r, "memcmp/equal/16", equal, 16);
  bench_run(&r, "memcmp/equal/64", equal, 64);
  bench_run(&r, "memcmp/equal/1024", equal, 1024);
  bench_run(&r, "memcmp/diff_first/8", diff_first, 8);
  bench_run(&r, "memcmp/diff_first/64", diff_first, 64);
  bench_run(&r, "memcmp/diff_last/8", diff_last, 8);
  bench_run(&r, "memcmp/diff_last/64", diff_last, 64);
  bench_run(&r, "memcmp/diff_last/1024", diff_last, 1024);
  return 0;
}
