set -ex

case $1 in
    "wasm32-wasi-ehpic" | "wasm32-wasi-eh" | "wasm32-wasi" | "all" )
        ;;
    *)
        echo "Usage: $0 wasm32-wasi-ehpic|wasm32-wasi-eh|wasm32-wasi|all"
        echo "       all will build all three targets and update the expected files"
        exit 1
        ;;
esac


function update_expected() {
    cp sysroot/share/wasm32-wasi/predefined-macros.txt expected/$1/predefined-macros.txt
    cp sysroot/share/wasm32-wasi/defined-symbols.txt expected/$1/defined-symbols.txt
    cp sysroot/share/wasm32-wasi/undefined-symbols.txt expected/$1/undefined-symbols.txt
    cp sysroot/share/wasm32-wasi/include-all.c expected/$1/include-all.c
}

function clean() {
    make clean
    rm -rf sysroot sysroot* || true
}

if test $1 = "all" ; then
    git submodule update --init --recursive

    clean
    bash build32.sh || true
    update_expected wasm32-wasi
    clean
    bash build32.sh

    clean
    bash build32-eh.sh || true
    update_expected wasm32-wasi-eh
    clean
    bash build32-eh.sh

    clean
    bash build32-ehpic.sh || true
    update_expected wasm32-wasi-ehpic
    clean
    bash build32-ehpic.sh

    exit 0
fi

update_expected $1
