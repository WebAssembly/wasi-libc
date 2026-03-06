# CMake's `execute_process` will run all commands specified concurrently but the
# stdout of each command is piped to the stdin of the next command. Tests print
# to stdout so that's not the behavior we want, so use `bash` to redirect stdout
# onto stderr.

set(CLIENTS)

if (NOT NCLIENTS)
  set(NCLIENTS 1)
endif()

foreach(i RANGE 1 ${NCLIENTS})
  # This is a bit tricky to setup, but the flow is:
  #
  # * The first line of output from the server is the port number that it's
  #   listening on.
  #
  # * This is read by the script below, and then printed again. In case it wasn't
  #   actually a port number, or if there's more clients, this means that the port
  #   will show up to the next client or print at the end if it's an error.
  #
  # * Next the shell's stdout is closed by putting stderr into the stdout slot.
  #   This closes stdout for the next client in the pipeline or the final cmake
  #   reader itself. Importantly this means that any prints of the client will
  #   show up on stderr which will make its way out of cmake.
  #
  # * Finally the client is run with the port as an argument.
  #
  # Overall this should handle cross-process synchronization to let the server bind
  # any arbitrary port, print out the port, and then thread that port to any number
  # of clients. All the while all other output from the tests is still surfaced in
  # CMake to debug and such. A bit baroque but we are scripting in CMake after all
  # so there's only so much that can be done.
  set(client_script "
read port  # read the first line of stdin from the previous process
echo $port # forward this line to the next process, or out to cmake itself
exec 2>&1  # close our stdout and replace it with stderr
cat <&0 &  # forward the rest of stdin to stderr so it shows up in cmake
exec ${ENGINE} -Wcomponent-model-async -Sp3,inherit-network ${CLIENT} \"$port\"
  ")
  list(APPEND CLIENTS COMMAND bash -c ${client_script})
endforeach()

execute_process(
  COMMAND ${ENGINE} -Wcomponent-model-async -Sp3,inherit-network ${SERVER}
  ${CLIENTS}
  TIMEOUT 5
  COMMAND_ERROR_IS_FATAL ANY
  COMMAND_ECHO STDOUT
)
