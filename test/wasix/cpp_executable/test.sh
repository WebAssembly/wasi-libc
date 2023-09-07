#!/bin/bash

wasmer run --verbose --enable-all ./main
RESULT=$?
if [ "$RESULT" != "123" ]; then
    echo "Test failed: different exit code ($RESULT vs. 123)" > /dev/stderr
    exit 1
fi
echo "cpp_executable test passed"