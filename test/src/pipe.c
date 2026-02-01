#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATTERNSIZE 16
static const char PATTERN[PATTERNSIZE] = {
    'A', 'B', 'C', 'D', '0', '1', '2', '3', 'a', 'b', 'c', 'd', 0, 1, 2, 3};

#define TEST(c, ...) ((c) ? 1 : (t_error(#c " failed: " __VA_ARGS__), 0))

int main(void) {
#ifdef __wasip3__ // pipe only works on wasip3 or up
  int pipefd[2];
  char buf[PATTERNSIZE];
  int res;

  TEST(pipe(pipefd) >= 0);

  TEST(write(pipefd[1], PATTERN, PATTERNSIZE) == PATTERNSIZE);

  TEST(read(pipefd[0], buf, PATTERNSIZE) == PATTERNSIZE);

  TEST(close(pipefd[0]) >= 0);
  TEST(close(pipefd[1]) >= 0);
#endif
  return 0;
}
