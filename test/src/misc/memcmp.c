//! add-flags.py(LDFLAGS): -Wl,--stack-first -Wl,--initial-memory=327680

#include <__macro_PAGESIZE.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int sign(int val) {
    return (0 < val) - (val < 0);
}
void test(char *ptr1, char *ptr2, size_t length, int want) {
  int got = memcmp(ptr1, ptr2, length);
  if (sign(got) != sign(want)) {
    printf("memcmp(%p, %p, %lu) = %d, want %d\n", ptr1, ptr2, length, got,
           want);
  }
}

int main(void) {
  char *const LIMIT = (char *)(__builtin_wasm_memory_size(0) * PAGESIZE);

  for (ptrdiff_t length = 0; length < 64; length++) {
    for (ptrdiff_t alignment = 0; alignment < 24; alignment++) {
      for (ptrdiff_t pos = -2; pos < length + 2; pos++) {
        // Create a buffer with the given length, at a pointer with the given
        // alignment. Using the offset LIMIT - PAGESIZE - 8 means many buffers
        // will straddle a (Wasm, and likely OS) page boundary.
        // The second buffer has a fixed address, which means it won't
        // always share alignment with first buffer.
        // Place the difference to find at every position in the buffers,
        // including just prior to it and after its end.
        char *ptr1 = LIMIT - PAGESIZE - 8 + alignment;
        char *ptr2 = LIMIT - PAGESIZE / 2;
        memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
        memset(ptr1, 5, length);
        memset(ptr2, 5, length);

        ptr1[pos] = 7;
        ptr2[pos] = 3;

        test(ptr1, ptr2, length,
             0 <= pos && pos < length ? ptr1[pos] - ptr2[pos] : 0);
        test(ptr2, ptr1, length,
             0 <= pos && pos < length ? ptr2[pos] - ptr1[pos] : 0);
      }
    }
  }

  return 0;
}
