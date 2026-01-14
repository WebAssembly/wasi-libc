#include <wasi/api.h>

#ifdef __wasip3__
// Waits for a subtask. Return value 1=complete 0=repeat -1=cancelled
// (Another option would be to directly return wasip3_subtask_state)
int wasip3_subtask_block_on(wasip3_subtask_status_t status);
#endif
