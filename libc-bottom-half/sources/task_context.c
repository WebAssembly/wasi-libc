#include <wasi/libc.h>

static _Thread_local void *task_context = NULL;

void *__wasilibc_task_context_get(void) { return task_context; }

void __wasilibc_task_context_set(void *ctx) { task_context = ctx; }
