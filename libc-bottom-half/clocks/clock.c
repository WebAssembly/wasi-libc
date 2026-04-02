#define _WASI_EMULATED_PROCESS_CLOCKS
#include <common/time.h>
#include <time.h>
#include <assert.h>

_Static_assert(CLOCKS_PER_SEC == NSEC_PER_SEC,
               "This implementation assumes that `clock` is in nanoseconds");

// Snapshot of the monotonic clock at the start of the program.
static struct timespec start;

// Use a priority of 10 to run fairly early in the implementation-reserved
// constructor priority range.
__attribute__((constructor(10))) static void init(void) {
  clock_gettime(CLOCK_MONOTONIC, &start);
}

// Define the libc symbol as `__clock` so that we can reliably call it
// from elsewhere in libc.
clock_t __clock(void) {
  // Use `MONOTONIC` instead of `PROCESS_CPUTIME_ID` since WASI doesn't have
  // an inherent concept of a process. Note that this means we'll incorrectly
  // include time from other processes, so this function is only declared by
  // the headers if `_WASI_EMULATED_PROCESS_CLOCKS` is defined.
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return (now.tv_sec - start.tv_sec) * 1E9 - start.tv_nsec + now.tv_nsec;
}

// Define a user-visible alias as a weak symbol.
__attribute__((__weak__, __alias__("__clock"))) clock_t clock(void);
