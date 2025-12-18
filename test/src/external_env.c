#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST(c, ...) ((c) || (t_error(#c " failed: " __VA_ARGS__), 0))

int main(int argc, char **argv) {
  char *s = getenv("VAR1");
  TEST(strcmp(s, "foo") == 0);
  s = getenv("VAR2");
  TEST(strcmp(s, "bar") == 0);

  return t_status;
}
