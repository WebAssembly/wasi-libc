#!/usr/bin/env bash

# Run a previously built test, generating in `$DIR`:
# - a `cmd.sh` script containing the actual command used to run the test in an
#   engine
# - a `fs` directory which the test may have used for file IO
# - an `output.log` file containing the output of the test
#
# Usage: DIR=... WASM=... ENGINE=... ./run-test.sh

ENGINE="${ENGINE:-wasmtime}"
[ -n "$WASM" ] || (echo "missing WASM variable" && exit 1)
[ -n "$DIR" ] || (echo "missing DIR variable" && exit 1)

cd $DIR
mkdir -p fs
echo "$ENGINE $WASM" > cmd.sh
chmod +x cmd.sh
./cmd.sh &> output.log
[ $? -eq 0 ] || echo "Test failed" >> output.log
