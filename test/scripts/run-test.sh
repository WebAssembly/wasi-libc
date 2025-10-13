#!/usr/bin/env bash

# Run a previously built test, generating in `$DIR`:
# - a `cmd.sh` script containing the actual command used to run the test in an
#   engine
# - a `fs` directory which the test may have used for file IO
# - an `output.log` file containing the output of the test
#
# Usage: ENV="..." DIR=... WASM=... ENGINE=... RUN="..." ARGS="..." ./run-test.sh

ENGINE="${ENGINE:-wasmtime}"
[ -n "$WASM" ] || (echo "missing WASM variable" && exit 1)
[ -n "$DIR" ] || (echo "missing DIR variable" && exit 1)

# Run the client/server sockets test, which requires a client and server
# running in separate processes
run_sockets_test_simple() {
    # Args are the same for client and server
    cd $DIR
    server_wasm=`echo $WASM | sed -e 's/client/server/g'`
    echo "$ENV $ENGINE $RUN $server_wasm $ARGS" > server_cmd.sh
    chmod +x server_cmd.sh
    # Start the server
    ./server_cmd.sh &> server_output.log &
    PID=$!
    [ $? -ne -1 ] || (echo "Failed to start server $server_wasm" && exit 1)
    echo "$ENV $ENGINE $RUN $WASM $ARGS" > cmd.sh
    chmod +x cmd.sh
    # Allow time for the server to start
    sleep 1
    # Start the client
    ./cmd.sh &> output.log
    test_result=$?
    # Server normally exits on its own, but kill it in case the test failed
    if [ ps -p $PID > /dev/null 2>&1 ]; then
        kill -9 $PID
    fi
    [ $test_result -eq 0 ] || echo "Test failed" >> output.log
}

# Run the client/server sockets test, which requires one server
# and multiple clients running in separate processes
CLIENTS=10
run_sockets_test_multiple() {
    # Args are the same for client and server
    cd $DIR
    server_wasm=`echo $WASM | sed -e 's/client/server/g'`
    echo "$ENV $ENGINE $RUN $server_wasm $ARGS" > server_cmd.sh
    chmod +x server_cmd.sh
    # Start the server
    ./server_cmd.sh &> server_output.log &
    PID=$!
    [ $? -ne -1 ] || (echo "Failed to start server $server_wasm" && exit 1)
    echo "$ENV $ENGINE $RUN $WASM $ARGS" > cmd.sh
    chmod +x cmd.sh
    # Allow time for the server to start
    sleep 1
    # Start the clients
    pids=()
    for ((i = 0; i < CLIENTS; i++)); do
        ./cmd.sh &> output"$i".log &
        pids+=( $! )
    done
    ANY_FAILURES=0
    for ((i = 0; i < CLIENTS; i++)); do
        wait ${pids[i]}
        if [ $? -ne 0 ]; then
           ANY_FAILURES=1
        fi
    done
    # Server normally exits on its own, but kill it in case the test failed
    if [ ps -p $PID > /dev/null 2>&1 ]; then
        kill -9 $PID
    fi
    [ $ANY_FAILURES -eq 0 ] || echo "Test failed" >> output.log
}

testname=$(basename $WASM)
if [ $testname == "sockets-server.component.wasm" ]; then
    exit 0
fi
if [ $testname == "sockets-multiple-server.component.wasm" ]; then
    exit 0
fi
if [ $testname == "sockets-client.component.wasm" ]; then
    run_sockets_test_simple
    exit $?
fi
if [ $testname == "sockets-multiple-client.component.wasm" ]; then
    run_sockets_test_multiple
    exit $?
fi
cd $DIR
mkdir -p fs
echo "$ENV $ENGINE $RUN $WASM $ARGS" > cmd.sh
chmod +x cmd.sh
./cmd.sh &> output.log
[ $? -eq 0 ] || echo "Test failed" >> output.log
