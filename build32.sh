#!/bin/bash -e
export TARGET_ARCH=wasm32
export TARGET_OS=wasix
cd tools/wasix-headers/WASI
git pull
cd ../../..
#cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc
make -j 14
