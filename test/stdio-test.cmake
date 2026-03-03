# TODO

execute_process(
  COMMAND ${ENGINE} -Sp3 -Wcomponent-model-async ${WASM} ${ARG} a
  COMMAND ${ENGINE} -Sp3 -Wcomponent-model-async ${WASM} ${ARG} b
  TIMEOUT 5
  COMMAND_ERROR_IS_FATAL ANY
  COMMAND_ECHO STDOUT
)
