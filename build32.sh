#!/bin/bash -e
export TARGET_ARCH=wasm32
export TARGET_OS=wasix
cd tools/wasix-headers/WASI
git reset --hard
git pull origin main || true
cd ../../..

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

make -j 14
rm -f sysroot/lib/wasm32-wasi/libc-printscan-long-double.a
rsync -rtvu --delete ./sysroot/ ./sysroot32/
