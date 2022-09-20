get_directory_property(DIR_COMPILE_OPTIONS COMPILE_OPTIONS)

add_custom_command(
  OUTPUT
    ${CMAKE_BINARY_DIR}/sysroot/share/${MULTIARCH_TRIPLE}/include-all.c

  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/sysroot/share/${MULTIARCH_TRIPLE}

  COMMAND
    ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/gen-predefined-macros.py
      --cc ${CMAKE_C_COMPILER}
      --output-directory ${SYSROOT_DIR}/share/${MULTIARCH_TRIPLE}
      --sysroot ${SYSROOT_DIR}
      -target ${CMAKE_C_COMPILER_TARGET} -O2 -DNDEBUG
      ${DIR_COMPILE_OPTIONS}

  DEPENDS
    ${PROJECT_SOURCE_DIR}/tools/gen-predefined-macros.py
)

add_custom_target(predefined-macros ALL
  DEPENDS
    ${CMAKE_BINARY_DIR}/sysroot/share/${MULTIARCH_TRIPLE}/include-all.c
)
add_dependencies(predefined-macros sysroot-headers)