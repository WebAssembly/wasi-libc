#include <stdlib.h>

void abort(void) {
    // WASI doesn't yet support signals, so just trap to halt the program.
    __builtin_trap();
}
