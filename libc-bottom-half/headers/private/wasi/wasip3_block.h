#ifndef WASI_WASIP3_BLOCK_H
#define WASI_WASIP3_BLOCK_H

#include <wasi/api.h>

#ifdef __wasip3__
#include <__typedef_ssize_t.h>
#include <wasi/descriptor_table.h> // for waitable_t

// Waits for a subtask to return
void wasip3_subtask_block_on(wasip3_subtask_status_t status);
// Waits for a (stream) transfer to complete
ssize_t wasip3_waitable_block_on(wasip3_waitable_status_t status, waitable_t stream);
#endif

#endif // WASI_WASIP3_BLOCK_H
