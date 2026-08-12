#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

// Emulated mmap allocates a header+body block, then fills it with pread for
// file-backed maps. On pread failure it must free that allocation, return
// MAP_FAILED, and leave errno set. Without free, repeated failures grow heap
// until mmap(MAP_ANON) fails with ENOMEM.
int main(void) {
  char tmp[] = "testsuite-mmap-pread-fail";
  int wr_fd;
  TEST((wr_fd = open(tmp, O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(wr_fd, "x", 1) == 1);

  // Single failing file-backed mmap: pread on O_WRONLY must fail.
  errno = 0;
  void *p = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, wr_fd, 0);
  int mmap_errno = errno;
  TEST(p == MAP_FAILED);
  TEST(mmap_errno != 0);

  // Leak stress: many failing mmaps of 1 MiB. With free, this stays small;
  // without free it exhausts wasm linear memory.
  const size_t map_len = 1u << 20;
  for (int i = 0; i < 256; i++) {
    errno = 0;
    p = mmap(NULL, map_len, PROT_READ, MAP_PRIVATE, wr_fd, 0);
    mmap_errno = errno;
    TEST(p == MAP_FAILED);
    TEST(mmap_errno != 0);
  }

  // After the failing path, MAP_ANON still works (memory was not leaked away).
  errno = 0;
  void *anon =
      mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  TEST(anon != MAP_FAILED);
  memset(anon, 0xab, 4096);
  TEST(munmap(anon, 4096) == 0);

  TEST(close(wr_fd) == 0);
  TEST(unlink(tmp) == 0);
  return t_status;
}
