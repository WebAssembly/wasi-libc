/*
 * https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/WebAssembly/WebAssemblyLowerEmscriptenEHSjLj.cpp
 */

#include <stdint.h>
#include <stdlib.h>

struct entry {
        uint32_t id;
        uint32_t label;
};

static _Thread_local struct state {
        uint32_t id;
        uint32_t size;
        struct arg {
                void *env;
                int val;
        } arg;
} g_state;

/*
 * table is allocated at the entry of functions which call setjmp.
 *
 *   table = malloc(40);
 *   size = 4;
 *   *(int *)table = 0;
 */
_Static_assert(sizeof(struct entry) * (4 + 1) <= 40, "entry size");

void *
saveSetjmp(void *env, uint32_t label, void *table, uint32_t size)
{
        struct state *state = &g_state;
        struct entry *e = table;
        uint32_t i;
        for (i = 0; i < size; i++) {
                if (e[i].id == 0) {
                        uint32_t id = ++state->id;
                        *(uint32_t *)env = id;
                        e[i].id = id;
                        e[i].label = label;
                        /*
                         * note: only the first word is zero-initialized
                         * by the caller.
                         */
                        e[i + 1].id = 0;
                        goto done;
                }
        }
        size *= 2;
        void *p = realloc(table, sizeof(*e) * (size + 1));
        if (p == NULL) {
                __builtin_trap();
        }
        table = p;
done:
        state->size = size;
        return table;
}

uint32_t
testSetjmp(unsigned int id, void *table, uint32_t size)
{
        struct entry *e = table;
        uint32_t i;
        for (i = 0; i < size; i++) {
                if (e[i].id == id) {
                        return e[i].label;
                }
        }
        return 0;
}

uint32_t
getTempRet0()
{
        struct state *state = &g_state;
        return state->size;
}

void
__wasm_longjmp(void *env, int val)
{
        struct state *state = &g_state;
        struct arg *arg = &state->arg;
        arg->env = env;
        arg->val = val;
        __builtin_wasm_throw(1, arg);
}
