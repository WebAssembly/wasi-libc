#include <stdio.h>
#include <sys/random.h>
#include <unistd.h>

int print_random_int_array() {
  ssize_t retval;
  int buf[100];
  retval = getrandom(buf, sizeof(int) * 100, 0);
  if (retval == -1) {
    return retval;
  }
  for (int i = 0; i < 100; ++i) {
    printf("%d ", buf[i]);
  }
  printf("\n");
  return 0;
}

int main() {
  int i = 0, retval = 0;
  while (i < 5) {
    printf("trial %d:\n     ", i);
    retval = print_random_int_array();
    printf("\n");
    if (retval == -1) {
      printf("failed!\n");
      return retval;
    }
    i++;
  }
  return 0;
}
