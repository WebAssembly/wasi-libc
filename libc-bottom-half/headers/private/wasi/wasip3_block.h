#ifndef WASI_WASIP3_BLOCK_H
#define WASI_WASIP3_BLOCK_H

#include <wasi/api.h>

#ifdef __wasip3__
// Waits for a subtask to return
void wasip3_subtask_block_on(wasip3_subtask_status_t status);
#endif

#endif // WASI_WASIP3_BLOCK_H
