#!/bin/bash
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR=$SCRIPT_DIR/../../test-builds
TESTS=
STATUS=0
FAILED_TESTS=""
PASSED_TESTS=""

if [ ! -z "${@:1}" ]; then
    TESTS=${@:1}
else
    TESTS=$(cd $SCRIPT_DIR && ls -d */)
fi

for test in $TESTS; do
    mkdir -p ${BUILD_DIR}/${test}
    cd ${BUILD_DIR}/${test}
    cmake --fresh -DCMAKE_MAKE_PROGRAM=make -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DCMAKE_SYSROOT=/opt/wasix-sysroot $SCRIPT_DIR/${test}
    cmake --build . --target all
    bash $SCRIPT_DIR/${test}/test.sh
    RESULT=$?
    if [ $RESULT -eq 0 ]; then
        PASSED_TESTS="$PASSED_TESTS $test"
    else
        FAILED_TESTS="$FAILED_TESTS $test"
        STATUS=1
    fi
done

echo "Passed tests: $PASSED_TESTS"
echo "Failed tests: $FAILED_TESTS"
exit $STATUS
