//! add-flags.py(LDFLAGS): -Wl,--stack-first -Wl,--initial-memory=327680

#define _GNU_SOURCE

#include <__macro_PAGESIZE.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void test(char *ptr, char *want) {
  char *got = strchrnul(ptr, 7);
  if (got != want) {
    printf("strchrnul(%p, 7) = %p, want %p\n", ptr, got, want);
  }
}

int main(void) {
  char *const LIMIT = (char *)(__builtin_wasm_memory_size(0) * PAGESIZE);

  for (ptrdiff_t length = 0; length < 64; length++) {
    for (ptrdiff_t alignment = 0; alignment < 24; alignment++) {
      for (ptrdiff_t pos = -2; pos < length + 2; pos++) {
        // Create a buffer with the given length, at a pointer with the given
        // alignment. Using the offset LIMIT - PAGESIZE - 8 means many buffers
        // will straddle a (Wasm, and likely OS) page boundary. Place the
        // character to find at every position in the buffer, including just
        // prior to it and after its end.
        char *ptr = LIMIT - PAGESIZE - 8 + alignment;
        memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
        memset(ptr, 5, length);

        // The first instance of the character is found.
        if (pos >= 0) ptr[pos + 2] = 7;
        ptr[pos] = 7;
        ptr[length] = 0;

        // The character is found if it's within range.
        test(ptr, 0 <= pos && pos < length ? &ptr[pos] : &ptr[length]);
      }
    }

    // We need space for the terminator.
    if (length == 0) continue;

    // Ensure we never read past the end of memory.
    char *ptr = LIMIT - length;
    memset(LIMIT - 2 * PAGESIZE, 0, 2 * PAGESIZE);
    memset(ptr, 5, length);

    ptr[length - 1] = 7;
    test(ptr, &ptr[length - 1]);

    ptr[length - 1] = 0;
    test(ptr, &ptr[length - 1]);
  }

  return 0;
}
