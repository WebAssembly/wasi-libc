add_custom_command(OUTPUT ${SYSROOT_LIB_DIR}/libc.imports
  COMMAND
    ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/gen-imports.py
    --nm ${CMAKE_NM}
    --libc-imports-output ${SYSROOT_LIB_DIR}/libc.imports
    --defined-symbols-output ${SYSROOT_SHARE_DIR}/defined-symbols.txt
    --undefined-symbols-output ${SYSROOT_SHARE_DIR}/undefined-symbols.txt
    $<TARGET_FILE:c>
    $<TARGET_FILE:c-printscan-long-double>
    $<TARGET_FILE:c-printscan-no-floating-point>
    $<TARGET_FILE:wasi-emulated-mman>
    $<TARGET_FILE:wasi-emulated-process-clocks>
    $<TARGET_FILE:wasi-emulated-getpid>
    $<TARGET_FILE:wasi-emulated-signal>
    $<TARGET_OBJECTS:crt1>
    $<TARGET_OBJECTS:crt1-command>
    $<TARGET_OBJECTS:crt1-reactor>
  DEPENDS
    ${PROJECT_SOURCE_DIR}/tools/gen-imports.py)

add_custom_target(libc.imports ALL
  DEPENDS ${SYSROOT_LIB_DIR}/libc.imports)
add_dependencies(libc.imports
  c
  c-printscan-long-double
  c-printscan-no-floating-point
  wasi-emulated-mman
  wasi-emulated-process-clocks
  wasi-emulated-getpid
  wasi-emulated-signal
  crt1
  crt1-command
  crt1-reactor
)