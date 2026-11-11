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
  bench_clobber(l);
  bench_clobber(r);
  bench_bytes(b, n);
  BENCH_LOOP(b) { BENCH_DO_NOT_OPTIMIZE(memcmp(l, r, n)); }
}

static void diff_first(bench_t *b) {
  size_t n = (size_t)b->arg;
  unsigned char buf_l[1040], buf_r[1040];
  unsigned char *l = buf_l + 1, *r = buf_r + 3;
  fill_pair(l, r, n);
  l[0] = 0x80;
  r[0] = 0x7f;
  bench_clobber(l);
  bench_clobber(r);
  BENCH_LOOP(b) { BENCH_DO_NOT_OPTIMIZE(memcmp(l, r, n)); }
}

static void diff_last(bench_t *b) {
  size_t n = (size_t)b->arg;
  unsigned char buf_l[1040], buf_r[1040];
  unsigned char *l = buf_l + 1, *r = buf_r + 3;
  fill_pair(l, r, n);
  l[n - 1] = 0x80;
  r[n - 1] = 0x7f;
  bench_clobber(l);
  bench_clobber(r);
  bench_bytes(b, n);
  BENCH_LOOP(b) { BENCH_DO_NOT_OPTIMIZE(memcmp(l, r, n)); }
}

BENCH_ARG(memcmp, equal, 4);
BENCH_ARG(memcmp, equal, 8);
BENCH_ARG(memcmp, equal, 16);
BENCH_ARG(memcmp, equal, 64);
BENCH_ARG(memcmp, equal, 1024);
BENCH_ARG(memcmp, diff_first, 8);
BENCH_ARG(memcmp, diff_first, 64);
BENCH_ARG(memcmp, diff_last, 8);
BENCH_ARG(memcmp, diff_last, 64);
BENCH_ARG(memcmp, diff_last, 1024);

int main(int argc, char **argv) { return bench_main(argc, argv); }
