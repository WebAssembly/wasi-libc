#include "bench.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(FILE *out, const char *prog) {
  fprintf(out,
          "usage: %s [--filter=SUBSTR] [--count=RUNS] [--ms=MSEC] [--help]\n",
          prog);
}

static long parse_positive_long(const char *flag, const char *s) {
  char *end = NULL;
  errno = 0;
  long val = strtol(s, &end, 10);
  if (errno != 0 || end == s || *end != '\0' || val <= 0) {
    fprintf(stderr, "invalid --%s value: '%s' (expected positive integer)\n",
            flag, s);
    exit(2);
  }
  return val;
}

bench_runner_t bench_init(int argc, char **argv) {
  static const struct option long_opts[] = {
      {"filter", required_argument, NULL, 'f'},
      {"count", required_argument, NULL, 'c'},
      {"ms", required_argument, NULL, 'm'},
      {"help", no_argument, NULL, 'h'},
      {NULL, 0, NULL, 0},
  };
  bench_runner_t r = {.filter = "", .count = 1, .ms = 100};
  optind = 1;
  int opt;
  while ((opt = getopt_long(argc, argv, "f:c:m:h", long_opts, NULL)) != -1) {
    switch (opt) {
    case 'f':
      r.filter = optarg;
      break;
    case 'c':
      r.count = parse_positive_long("count", optarg);
      break;
    case 'm':
      r.ms = parse_positive_long("ms", optarg);
      break;
    case 'h':
      usage(stdout, argv[0]);
      exit(0);
    default:
      usage(stderr, argv[0]);
      exit(2);
    }
  }
  if (optind < argc) {
    fprintf(stderr, "unexpected argument: '%s'\n", argv[optind]);
    usage(stderr, argv[0]);
    exit(2);
  }
  return r;
}

void bench_run(const bench_runner_t *r, const char *name, void (*fn)(bench_t *),
               intptr_t arg) {
  if (!strstr(name, r->filter))
    return;

  const double target_ns = (double)r->ms * 1e6;
  const double max_n = 1e9;

  for (long c = 0; c < r->count; c++) {
    bench_t b = {.n = 1, .arg = arg};
    for (;;) {
      fn(&b);
      if ((double)b.ns >= target_ns || (double)b.n >= max_n)
        break;

      // Extrapolate the iteration count needed to reach target_ns from the
      // observed rate (b.n / b.ns). This follows Go's predictN scaling
      // (https://go.dev/src/testing/benchmark.go) logic in a few ways:
      // - Aim 20% past target_ns to ensure a slow prior round doesn't regress
      //   to the mean, undershoot target_ns, and cause us to do another pass.
      // - Cap each step at 100x growth so coarse early measurements ramp up
      //   gradually (1 -> 100 -> 10000 -> ...).
      // - Always grow by at least 1 so b.n doesn't get stuck.
      double elapsed_ns = b.ns > 0 ? (double)b.ns : 1.0;
      double growth = 1.2 * target_ns / elapsed_ns;
      if (growth > 100.0)
        growth = 100.0;
      double next_n = (double)b.n * growth + 1.0;
      if (next_n > max_n)
        next_n = max_n;
      b = (bench_t){.n = (size_t)next_n, .arg = arg};
    }

    double ns_op = (double)b.ns / (double)b.n;
    printf("Benchmark_%-24s\t%10zu\t%10.2f ns/op", name, b.n, ns_op);
    if (b.bytes && ns_op > 0)
      printf("\t%10.2f MB/s", (double)b.bytes * 1e3 / ns_op);
    putchar('\n');
    fflush(stdout);
  }
}
