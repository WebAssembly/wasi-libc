# CMake's `execute_process` will run all commands specified concurrently but the
# stdout of each command is piped to the stdin of the next command. Tests print
# to stdout so that's not the behavior we want, so use `bash` to redirect stdout
# onto stderr.

set(CLIENTS)

if (NOT NCLIENTS)
  set(NCLIENTS 1)
endif()

foreach(i RANGE 1 ${NCLIENTS})
  list(APPEND CLIENTS COMMAND bash -c "exec ${ENGINE} -Sinherit-network ${CLIENT} 1>&2")
endforeach()

execute_process(
  COMMAND bash -c "exec ${ENGINE} -Sinherit-network ${SERVER} 1>&2"
  ${CLIENTS}
  TIMEOUT 5
  COMMAND_ERROR_IS_FATAL ANY
  COMMAND_ECHO STDOUT
)
