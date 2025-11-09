# CMake's `execute_process` will run all commands specified concurrently but the
# stdout of each command is piped to the stdin of the next command. Tests print
# to stdout so that's not the behavior we want, so use `bash` to redirect stdout
# onto stderr.
#
# Also issue a `sleep 1` as a bit of a kludge to wait for the server to come
# online at this time.

set(CLIENTS)

if (NOT NCLIENTS)
  set(NCLIENTS 1)
endif()

foreach(i RANGE 1 ${NCLIENTS})
  list(APPEND CLIENTS COMMAND bash -c "sleep 1 && ${ENGINE} -Sinherit-network ${CLIENT} 1>&2")
endforeach()

execute_process(
  COMMAND bash -c "${ENGINE} -Sinherit-network ${SERVER} 1>&2"
  ${CLIENTS}
  TIMEOUT 5
  COMMAND_ERROR_IS_FATAL ANY
  COMMAND_ECHO STDOUT
)
