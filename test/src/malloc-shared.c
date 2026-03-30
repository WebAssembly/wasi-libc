#include "test.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <__macro_PAGESIZE.h>

__attribute__((__export_name__("wasi:cli/run@0.2.0#run")))
bool exports_wasi_cli_run_run(void) {
    void* p = malloc(7777777);
    if (p == NULL) {
        t_error("malloc failed\n");
    } else {
        free(p);
    }

    return t_status != 0;
}
