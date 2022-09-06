CC=clang
LLD_PATH=/prog/rust/build/x86_64-unknown-linux-gnu/lld/bin

CFLAGS = --target=wasm32-wasmer-wasi \
         -O2 \
         --sysroot ../../wasix-libc/sysroot32 \
         -matomics \
         -mbulk-memory \
         -mmutable-globals \
         -pthread \
         -mthread-model posix \
         -ftls-model=local-exec \
         -fno-trapping-math \
         -D_WASI_EMULATED_MMAN \
         -D_WASI_EMULATED_SIGNAL \
		 -D_WASI_EMULATED_PROCESS_CLOCKS \
         -Wall \
         -Wextra \
         -Werror \
         -Wno-null-pointer-arithmetic \
         -Wno-unused-parameter \
         -Wno-sign-compare \
         -Wno-unused-variable \
         -Wno-unused-function \
         -Wno-ignored-attributes \
         -Wno-missing-braces \
         -Wno-ignored-pragmas \
         -Wno-unused-but-set-variable \
         -Wno-unknown-warning-option \
         -Wno-parentheses \
         -Wno-shift-op-parentheses \
         -Wno-bitwise-op-parentheses \
         -Wno-logical-op-parentheses \
         -Wno-string-plus-int \
         -Wno-dangling-else \
         -Wno-unknown-pragmas \
         -MD \
         -MP

CLFLAGS = -Wl,--shared-memory \
          -Wl,--max-memory=4294967296 \
          -Wl,--import-memory \
          -Wl,--export-dynamic \
          -Wl,--export=__heap_base \
          -Wl,--export=__stack_pointer \
          -Wl,--export=__data_end \
          -Wl,--export=__wasm_init_tls \
          -Wl,--export=__tls_size \
          -Wl,--export=__tls_align \
          -Wl,--export=__tls_base

SRC=$(wildcard *.c)

export PATH := $(LLD_PATH):$(PATH)

all: longjmp file-copy

%: %.c
	mkdir -p ../dist/host
	clang $@.c -o ../dist/host/$@
	mkdir -p ../dist/wasix
	clang $(CFLAGS) $(CLFLAGS) \
              $@.c \
              -o ../dist/wasix/$@.wasm

clean:
	rm -f *.o
	rm -f *.s
	rm -f *.ll
	rm -f *~
	rm -f ../dist/host/*
	rm -f ../dist/wasix/*
