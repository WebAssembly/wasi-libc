#include "test.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int close_input_writer(void) {
  TEST(write(1, "hello world\n", 12) == 12);
  TEST(close(1) == 0);
  return t_status;
}

static int close_input_reader(void) {
  char buf[13];
  ssize_t n = read(0, buf, 12);
  TEST(n == 12);
  buf[12] = '\0';
  TEST(strcmp(buf, "hello world\n") == 0);

  // further reads should fail as there's nothing left
  TEST(read(0, buf, 12) == 0);
  TEST(close(0) == 0);
  return t_status;
}

static int close_output_writer(void) {
  while (write(1, "hello world\n", 12) == 12) {
    // ... write again on the next iteration ...
  }
  TEST(close(1) == 0);
  return t_status;
}

static int close_output_reader(void) {
  // Read one message successfully...
  char buf[13];
  ssize_t n = read(0, buf, 12);
  TEST(n == 12);
  buf[12] = '\0';
  TEST(strcmp(buf, "hello world\n") == 0);

  // ... but then close out stdin to let the other end know we're hanging up.
  TEST(close(0) == 0);
  return t_status;
}

int main(int argc, char **argv) {
  TEST(argc == 3);
  bool writer = argv[2][0] == 'a';

  if (strcmp(argv[1], "close-input") == 0) {
    if (writer)
      return close_input_writer();
    return close_input_reader();
  }

  if (strcmp(argv[1], "close-output") == 0) {
    if (writer)
      return close_output_writer();
    return close_output_reader();
  }

  printf("unknown test: %s\n", argv[1]);
  return 1;
}
