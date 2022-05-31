#!/bin/bash -e
export TARGET_ARCH=wasm64
export TARGET_OS=wasix
cd tools/wasix-headers/WASI
git pull
cd ../../..
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc --64bit
make -j 14
rsync -rtvu --delete ./sysroot/ ./sysroot64/
