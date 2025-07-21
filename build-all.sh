#! /bin/bash

set -euxo pipefail

export CHECK_SYMBOLS=no

make clean-all
./build32.sh
make clean
./build32-eh.sh
make clean
./build32-ehpic.sh

rsync -Lrtv --delete ./sysroot32/ ~/.wasix-sysroot/sysroot
rsync -Lrtv --delete ./sysroot32-eh/ ~/.wasix-sysroot/sysroot-eh
rsync -Lrtv --delete ./sysroot32-ehpic/ ~/.wasix-sysroot/sysroot-ehpic
