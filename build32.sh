#!/bin/bash

set -Eeuxo pipefail

export TARGET_ARCH=wasm32
export TARGET_OS=wasix

# Build the compiler runtime lib
mkdir -p build/compiler-rt
cd build/compiler-rt
cmake \
    -DCMAKE_SYSTEM_NAME=WASI \
    -DCMAKE_SYSTEM_VERSION=1 \
    -DCMAKE_SYSTEM_PROCESSOR=wasm32 \
    -DCMAKE_BUILD_TYPE=RelWithDebugInfo \
    -DCMAKE_C_COMPILER_WORKS=ON \
    -DCMAKE_CXX_COMPILER_WORKS=ON \
    -DCMAKE_C_LINKER_DEPFILE_SUPPORTED=OFF \
    -DCMAKE_CXX_LINKER_DEPFILE_SUPPORTED=OFF \
    -DCOMPILER_RT_BAREMETAL_BUILD=ON \
    -DCOMPILER_RT_BUILD_XRAY=OFF \
    -DCOMPILER_RT_INCLUDE_TESTS=OFF \
    -DCOMPILER_RT_HAS_FPIC_FLAG=OFF \
    -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
    -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
    -DCOMPILER_RT_BUILD_XRAY=OFF \
    -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
    -DCOMPILER_RT_BUILD_PROFILE=OFF \
    -DCOMPILER_RT_BUILD_CTX_PROFILE=OFF \
    -DCOMPILER_RT_BUILD_MEMPROF=OFF \
    -DCOMPILER_RT_BUILD_ORC=OFF \
    -DCOMPILER_RT_BUILD_GWP_ASAN=OFF \
    -DCOMPILER_RT_USE_LLVM_UNWINDER=OFF \
    -DSANITIZER_USE_STATIC_LLVM_UNWINDER=OFF \
    -DCOMPILER_RT_ENABLE_STATIC_UNWINDER=OFF \
    -DHAVE_UNWIND_H=OFF \
    -DCOMPILER_RT_HAS_FUNWIND_TABLES_FLAG=OFF \
    -DCMAKE_C_COMPILER_TARGET=wasm32-wasi \
    -DCOMPILER_RT_OS_DIR=wasm32-wasi \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/../../tools/clang-wasix.cmake_toolchain \
    -DCMAKE_SYSROOT=$(pwd)/../../sysroot \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../../sysroot \
    -DUNIX:BOOL=ON \
    ../../tools/llvm-project/compiler-rt
cmake --build . --target install --parallel 16
llvm-ranlib ../../sysroot/lib/wasm32-wasi/libclang_rt.builtins-wasm32.a
cd ../..

# Build the extensions
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc
cp -f libc-bottom-half/headers/public/wasi/api.h libc-bottom-half/headers/public/wasi/api_wasix.h
sed -i 's|__wasi__|__wasix__|g' libc-bottom-half/headers/public/wasi/api_wasix.h
sed -i 's|__wasi_api_h|__wasix_api_h|g' libc-bottom-half/headers/public/wasi/api_wasix.h
cp -f libc-bottom-half/sources/__wasilibc_real.c libc-bottom-half/sources/__wasixlibc_real.c

# Build WASI
cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
cp -f libc-bottom-half/headers/public/wasi/api.h libc-bottom-half/headers/public/wasi/api_wasi.h

# Emit the API header
cat > libc-bottom-half/headers/public/wasi/api.h<<EOF
#include "api_wasi.h"
#include "api_wasix.h"
#include "api_poly.h"
EOF

make -j 16
rm -f sysroot/lib/wasm32-wasi/libc-printscan-long-double.a

# Build C++ sysroot
mkdir -p build/libcxx
cd build/libcxx
cmake \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/../../tools/clang-wasix.cmake_toolchain \
    -DCMAKE_SYSROOT=$(pwd)/../../sysroot \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../../sysroot \
    -DCXX_SUPPORTS_CXX23=ON \
    -DLIBCXX_ENABLE_THREADS:BOOL=ON \
    -DLIBCXX_HAS_PTHREAD_API:BOOL=ON \
    -DLIBCXX_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
    -DLIBCXX_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
    -DLIBCXX_HAS_WIN32_THREAD_API:BOOL=OFF \
    -DCMAKE_BUILD_TYPE=RelWithDebugInfo \
    -DLIBCXX_ENABLE_SHARED:BOOL=OFF \
    -DLIBCXX_ENABLE_EXPERIMENTAL_LIBRARY:BOOL=OFF \
    -DLIBCXX_ENABLE_EXCEPTIONS:BOOL=OFF \
    -DLIBCXX_ENABLE_FILESYSTEM:BOOL=OFF \
    -DLIBCXX_CXX_ABI=libcxxabi \
    -DLIBCXX_HAS_MUSL_LIBC:BOOL=ON \
    -DLIBCXX_ABI_VERSION=2 \
    -DLIBCXXABI_ENABLE_EXCEPTIONS:BOOL=OFF \
    -DLIBCXXABI_ENABLE_SHARED:BOOL=OFF \
    -DLIBCXXABI_SILENT_TERMINATE:BOOL=ON \
    -DLIBCXXABI_ENABLE_THREADS:BOOL=ON \
    -DLIBCXXABI_HAS_PTHREAD_API:BOOL=ON \
    -DLIBCXXABI_HAS_EXTERNAL_THREAD_API:BOOL=OFF \
    -DLIBCXXABI_BUILD_EXTERNAL_THREAD_LIBRARY:BOOL=OFF \
    -DLIBCXXABI_HAS_WIN32_THREAD_API:BOOL=OFF \
    -DLIBCXXABI_ENABLE_PIC:BOOL=OFF \
    -DLIBCXXABI_USE_LLVM_UNWINDER:BOOL=OFF \
    -DCMAKE_C_COMPILER_WORKS=ON \
    -DCMAKE_CXX_COMPILER_WORKS=ON \
    -DLLVM_COMPILER_CHECKED=ON \
    -DUNIX:BOOL=ON \
    -DLIBCXX_LIBDIR_SUFFIX=/wasm32-wasi \
    -DLIBCXXABI_LIBDIR_SUFFIX=/wasm32-wasi \
    -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" \
    ../../tools/llvm-project/runtimes
cmake --build . --target install --parallel 16
cd ../..

rsync -Lrtv --delete ./sysroot/ ./sysroot32/
