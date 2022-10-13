#!/bin/bash -e
export TARGET_ARCH=wasm64
export TARGET_OS=wasix
cd tools/wasix-headers/WASI
git pull origin main || true
cd ../../..
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc --64bit
make -j 14
rm -f sysroot/lib/wasm32-wasi/libc-printscan-long-double.a
rsync -rtvu --delete ./sysroot/ ./sysroot64/
