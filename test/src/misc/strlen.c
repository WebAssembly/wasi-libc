//! add-flags.py(LDFLAGS): -Wl,--stack-first -Wl,--initial-memory=327680

#include <__macro_PAGESIZE.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void test(char *ptr, size_t want) {
  size_t got = strlen(ptr);
  if (got != want) {
    printf("strlen(%p) = %lu, want %lu\n", ptr, got, want);
  }
}

int main(void) {
  char *const LIMIT = (char *)(__builtin_wasm_memory_size(0) * PAGESIZE);

  for (size_t length = 0; length < 64; length++) {
    for (size_t alignment = 0; alignment < 24; alignment++) {
      char *ptr = LIMIT - PAGESIZE + alignment;
      memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
      memset(ptr, 5, length);
      test(ptr, length);

      ptr[-1] = 5;
      test(ptr, length);
    }

    char *ptr = LIMIT - length - 1;
    memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
    memset(ptr, 5, length);
    test(ptr, length);
  }

  return 0;
}
