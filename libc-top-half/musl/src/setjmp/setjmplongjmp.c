#ifndef __wasilibc_unmodified_upstream
#  ifdef __wasm_exception_handling__

/*
 * function prototypes
 */
void __wasm_setjmp(void *env, unsigned int label, void *func_invocation_id);
unsigned int __wasm_setjmp_test(void *env, void *func_invocation_id);
void __wasm_longjmp(void *env, int val);

/*
 * this is a temorary storage used by the communication between
 * __wasm_sjlj_longjmp and WebAssemblyLowerEmscriptenEHSjL-generated
 * logic.
 * ideally, this can be replaced with multivalue.
 */
struct arg {
		void *env;
		int val;
};

/*
 * jmp_buf should have large enough size and alignment to contain
 * this structure.
 */
struct jmp_buf_impl {
        void *func_invocation_id;
        unsigned int label;

        struct arg arg;
};

void
__wasm_setjmp(void *env, unsigned int label, void *func_invocation_id)
{
        struct jmp_buf_impl *jb = (struct jmp_buf_impl *)env;
        if (label == 0) { /* ABI contract */
                __builtin_trap();
        }
        if (func_invocation_id == 0) { /* sanity check */
                __builtin_trap();
        }
        jb->func_invocation_id = func_invocation_id;
        jb->label = label;
}

unsigned int
__wasm_setjmp_test(void *env, void *func_invocation_id)
{
        struct jmp_buf_impl *jb = (struct jmp_buf_impl *)env;
        if (jb->label == 0) { /* ABI contract */
                __builtin_trap();
        }
        if (func_invocation_id == 0) { /* sanity check */
                __builtin_trap();
        }
        if (jb->func_invocation_id == func_invocation_id) {
                return jb->label;
        }
        return 0;
}

void
__wasm_longjmp(void *env, int val)
{
        struct jmp_buf_impl *jb = (struct jmp_buf_impl *)env;
        struct arg *arg = (struct arg *)&jb->arg;
        /*
         * C standard says:
         * The longjmp function cannot cause the setjmp macro to return
         * the value 0; if val is 0, the setjmp macro returns the value 1.
         */
        if (val == 0) {
                val = 1;
        }
        arg->env = env;
        arg->val = val;
        __builtin_wasm_throw(1, arg); /* 1 == C_LONGJMP */
}

#  else

#include <setjmp.h>
#include <wasi/libc.h>

_Noreturn void longjmp (jmp_buf buf, int val) {
    __wasilibc_longjmp(buf, val);
}

int setjmp (jmp_buf buf) {
    return __wasilibc_setjmp(buf);
}

// ignoring signal masking for now
int sigsetjmp(jmp_buf  buf, int savesigs) {
    return __wasilibc_setjmp(buf);
}

#  endif
#endif
