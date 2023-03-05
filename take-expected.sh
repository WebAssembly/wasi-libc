#/bin/bash -e
cp -f sysroot/share/wasm32-wasi/defined-symbols.txt expected/wasm32-wasi/defined-symbols.txt
cp -f sysroot/share/wasm32-wasi/include-all.c expected/wasm32-wasi/include-all.c
cp -f sysroot/share/wasm32-wasi/predefined-macros.txt expected/wasm32-wasi/predefined-macros.txt
cp -f sysroot/share/wasm32-wasi/undefined-symbols.txt expected/wasm32-wasi/undefined-symbols.txt

cp -f sysroot/share/wasm64-wasi/defined-symbols.txt expected/wasm64-wasi/defined-symbols.txt
cp -f sysroot/share/wasm64-wasi/include-all.c expected/wasm64-wasi/include-all.c
cp -f sysroot/share/wasm64-wasi/predefined-macros.txt expected/wasm64-wasi/predefined-macros.txt
cp -f sysroot/share/wasm64-wasi/undefined-symbols.txt expected/wasm64-wasi/undefined-symbols.txt
