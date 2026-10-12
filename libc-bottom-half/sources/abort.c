#include "wasi/report-error.h"
#include <stdlib.h>

void abort(void) {
  // wasm doesn't support signals, so just trap to halt the program.
#ifdef __webp2__
  __wasilibc_webp2_flush_stdio();
#endif
  __builtin_trap();
}
