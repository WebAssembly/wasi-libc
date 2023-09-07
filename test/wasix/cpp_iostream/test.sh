#!/bin/bash

wasmer run --verbose --enable-all ./main
RESULT=$?
if [ "$RESULT" != "0" ]; then
    echo "Test failed: different exit code ($RESULT vs. 0)" > /dev/stderr
    exit 1
fi
echo "cpp_iostream test passed"