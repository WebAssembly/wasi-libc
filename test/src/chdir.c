#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

static int exists(const char *path) {
  struct stat st;
  return stat(path, &st) == 0;
}

static void spray_heap(void) {
  #define N 100
  void *ptrs[N];
  for (int i = 0; i < N; i++) {
    size_t size = (rand() % 256) + 16;
    ptrs[i] = malloc(size);
    memset(ptrs[i], 0x42, size);
  }
  for (int i = 0; i < N; i++) {
    free(ptrs[i]);
  }
}

int main(void) {
  spray_heap();

  int fd;
  TEST(mkdir("a", 0755) == 0);
  TEST(mkdir("a/b", 0755) == 0);
  TEST(mkdir("a/b/c", 0755) == 0);

  TEST((fd = open("a1.txt", O_CREAT | O_WRONLY, 0644)) >= 0);
  TEST(close(fd) == 0);
  TEST((fd = open("a/a2.txt", O_CREAT | O_WRONLY, 0644)) >= 0);
  TEST(close(fd) == 0);
  TEST((fd = open("a/b/a3.txt", O_CREAT | O_WRONLY, 0644)) >= 0);
  TEST(close(fd) == 0);
  TEST((fd = open("a/b/c/a4.txt", O_CREAT | O_WRONLY, 0644)) >= 0);
  TEST(close(fd) == 0);

  TEST(exists("a1.txt"));
  TEST(exists("a/a2.txt"));
  TEST(exists("a/b/a3.txt"));
  TEST(exists("a/b/c/a4.txt"));

  TEST(chdir("a") == 0);
  TEST(exists("../a1.txt"));
  TEST(exists("a2.txt"));
  TEST(exists("b/a3.txt"));
  TEST(exists("b/c/a4.txt"));

  TEST(chdir("b") == 0);
  TEST(exists("../../a1.txt"));
  TEST(exists("../a2.txt"));
  TEST(exists("a3.txt"));
  TEST(exists("c/a4.txt"));

  TEST(chdir("c") == 0);
  TEST(exists("../../../a1.txt"));
  TEST(exists("../../a2.txt"));
  TEST(exists("../a3.txt"));
  TEST(exists("a4.txt"));

  TEST(chdir("..") == 0);
  TEST(exists("../../a1.txt"));
  TEST(exists("../a2.txt"));
  TEST(exists("a3.txt"));
  TEST(exists("c/a4.txt"));

  TEST(chdir("..") == 0);
  TEST(exists("../a1.txt"));
  TEST(exists("a2.txt"));
  TEST(exists("b/a3.txt"));
  TEST(exists("b/c/a4.txt"));

  TEST(chdir("..") == 0);
  TEST(exists("a1.txt"));
  TEST(exists("a/a2.txt"));
  TEST(exists("a/b/a3.txt"));
  TEST(exists("a/b/c/a4.txt"));

  TEST(chdir("a/b") == 0);
  TEST(exists("../../a1.txt"));
  TEST(exists("../a2.txt"));
  TEST(exists("a3.txt"));
  TEST(exists("c/a4.txt"));

  return t_status;
}
