#!/bin/bash
set -ueo pipefail

# Configuration
clang="${1:-/opt/wasi-sdk/bin/clang}"
ar="${2:-/opt/wasi-sdk/bin/llvm-ar}"
runner="${3:-wasmtime}"

# Generate options.h
make src/common/options.h CC="$clang"

# Build the common source files.
rm -f *.o
"$clang" -c -O2 -D_WASI_EMULATED_MMAN \
    -Werror=incompatible-pointer-types \
    -Werror=missing-declarations \
    -Werror=implicit-function-declaration \
    src/common/*.c
"$ar" cr libcommon.a *.o
rm -f *.o

:> full.log
:> pass-fail.log

# Compile the API tests
for i in src/api/*.c; do
    if [ "$i" != src/api/main.c ]; then
        echo "API compile and link test $i..." | tee -a pass-fail.log

        # Add -Dstatic= to force static functions to be externally visible so
        # that the aren't optimized away, so that we test whether the things
        # they reference actually exist.
        "$clang" \
            "$i" src/api/main.c \
            -I src/common \
            -Dstatic= \
            -D_WASI_EMULATED_MMAN -D_ALL_SOURCE \
            -Werror=incompatible-pointer-types \
            -Werror=missing-declarations \
            -Werror=implicit-function-declaration \
            -lwasi-emulated-mman -lc-printscan-long-double && \
        (echo " - PASS" | tee -a pass-fail.log) || \
        (echo " - FAIL" | tee -a pass-fail.log)
    fi
done 2>&1 | tee -a full.log

# Compile and run the execution tests
for i in src/{functional,math,musl,regression}/*.c; do
    echo "Execution test $i..." | tee -a pass-fail.log
    "$clang" \
        -Wno-unknown-pragmas \
        "$i" libcommon.a \
        -I src/common \
        -D_WASI_EMULATED_MMAN -D_ALL_SOURCE \
        -lc-printscan-long-double -lwasi-emulated-mman && \
    "$runner" a.out && \
    (echo " - PASS" | tee -a pass-fail.log) || \
    (echo " - FAIL" | tee -a pass-fail.log)
done 2>&1 | tee -a full.log

rm -f a.out

diff -u expected.log pass-fail.log
echo "Everything looks normal!"
