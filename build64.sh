#!/bin/bash

set -Eeuxo pipefail

export TARGET_ARCH=wasm64
export TARGET_OS=wasix
cd tools/wasix-headers/WASI
git reset --hard
git pull origin main || true
cd ../../..

# Build the extensions
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc --64bit
cp -f libc-bottom-half/headers/public/wasi/api.h libc-bottom-half/headers/public/wasi/api_wasix.h
sed -i 's|__wasi__|__wasix__|g' libc-bottom-half/headers/public/wasi/api_wasix.h
sed -i 's|__wasi_api_h|__wasix_api_h|g' libc-bottom-half/headers/public/wasi/api_wasix.h
cp -f libc-bottom-half/sources/__wasilibc_real.c libc-bottom-half/sources/__wasixlibc_real.c

# Build WASI
mkdir -p build/temp
rsync -rtvu --delete tools/wasix-headers/ build/temp
cp -r -f tools/wasi-headers/WASI/phases/* build/temp/WASI/phases
mv -f build/temp/WASI/phases/snapshot/witx/wasi_snapshot_preview1.witx build/temp/WASI/phases/snapshot/witx/wasix_v1.witx
sed -i 's|(field $buf_len $size)|(field $buf_len usize)|g' build/temp/WASI/phases/snapshot/witx/typenames.witx
sed -i 's|(param $buf_len $size)|(param $buf_len usize)|g' build/temp/WASI/phases/snapshot/witx/wasix_v1.witx
cargo clean --manifest-path build/temp/Cargo.toml
cargo run --manifest-path build/temp/Cargo.toml generate-libc --64bit
cp -f libc-bottom-half/headers/public/wasi/api.h libc-bottom-half/headers/public/wasi/api_wasi.h

# Emit the API header
cat > libc-bottom-half/headers/public/wasi/api.h<<EOF
#include "api_wasi.h"
#include "api_wasix.h"
#include "api_poly.h"
EOF

make -j 14
rm -f sysroot/lib/wasm32-wasi/libc-printscan-long-double.a
rsync -Lrtv --delete ./sysroot/ ./sysroot64/
