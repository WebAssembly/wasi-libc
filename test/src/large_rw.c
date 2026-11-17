#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// The component model's per-operation stream limit (`Buffer.MAX_LENGTH`).
// On WASIp3 a single stream read/write of more elements than this traps, so
// libc must clamp oversized requests into short reads/writes instead.
//
// Only WASIp3 has this limit, so the checks below only run there. Other
// targets hit unrelated host limits on a buffer this large: Wasmtime's WASIp1
// returns ENOMEM, and its WASIp2 traps on hostcall fuel in `pwrite`.
#define MAX_LENGTH (((size_t)1 << 28) - 1)
#define LEN (MAX_LENGTH + 1)

static int short_count_ok(ssize_t n) {
  return n > 0 && (size_t)n <= MAX_LENGTH;
}

int main(void) {
#if defined(__wasip1__) || defined(__wasip2__) || defined(__webp2__)
  if (1)
    return t_status;
#endif

  char tmp[] = "testsuite-large-rw";
  char *buf = malloc(LEN);
  TEST(buf != NULL);
  if (buf == NULL)
    return t_status;

  int fd;
  TEST((fd = open(tmp, O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, "Hello, world!", 13) == 13);

  // On WASIp3 the main check is that none of the calls below trap: without
  // clamping, the host traps on the oversized stream operation before any
  // count is returned. The count checks are secondary.

  // Oversized reads of a small file return what's there rather than trapping.
  TEST(pread(fd, buf, LEN, 0) == 13);
  TEST(memcmp(buf, "Hello, world!", 13) == 0);
  memset(buf, 0, 13);
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(read(fd, buf, LEN) == 13);
  TEST(memcmp(buf, "Hello, world!", 13) == 0);

  // Oversized writes succeed with a (possibly short) count.
  memset(buf, 'x', LEN);
  ssize_t n, written;
  TEST(lseek(fd, 0, SEEK_SET) == 0);
  TEST(short_count_ok(written = write(fd, buf, LEN)));
  TEST(lseek(fd, 0, SEEK_CUR) == written);

  // Same for `pwrite`, which must also not move the cursor. A distinct fill
  // byte shows the bytes it reports as written really landed in the file.
  memset(buf, 'y', LEN);
  TEST(short_count_ok(n = pwrite(fd, buf, LEN, 1)));
  TEST(lseek(fd, 0, SEEK_CUR) == written);
  char tail[4];
  if (n >= (ssize_t)sizeof(tail)) {
    off_t tail_off = (off_t)1 + n - (off_t)sizeof(tail);
    TEST(pread(fd, tail, sizeof(tail), tail_off) == sizeof(tail));
    TEST(memcmp(tail, "yyyy", sizeof(tail)) == 0);
  }

  free(buf);
  TEST(close(fd) == 0);
  TEST(unlink(tmp) == 0);
  return t_status;
}
