#!/usr/bin/env bash

# Create stub tests in `$TO_DIR` that `#include` a test in the `$FROM_DIR`
# directory. If the stub already exists, do not overwrite it.
#
# Usage: FROM_DIR=... TO_DIR=... ./generate-stubs.sh

FROM_DIR="${FROM_DIR:-build/download/libc-test}"
TO_DIR="${TO_DIR:-src/libc-test}"
# For now, only retrieve the functional tests.
FUNCTIONAL_TESTS=$(find $FROM_DIR/src/math -name '*.c')

for from in $FUNCTIONAL_TESTS; do
    to="${from/"$FROM_DIR/src"/"$TO_DIR"}"
	if [ ! -f $to ]; then \
		mkdir -p $(dirname $to)
		echo '// add-flags.py(CFLAGS): -I.' > $to
		echo "#include \"$from\"" >> $to
	fi
done