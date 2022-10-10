file(GLOB
  LIBC_BOTTOM_HALF_CRT_SOURCES
  CONFIGURE_DEPENDS
  ${LIBC_BOTTOM_HALF_DIR}/crt/*.c)

foreach(crt_path ${LIBC_BOTTOM_HALF_CRT_SOURCES})
  get_filename_component(crt_name ${crt_path} NAME_WLE)
  add_library(${crt_name} OBJECT ${crt_path})

  target_include_directories(${crt_name}
    PRIVATE
      ${LIBC_BOTTOM_HALF_INC_DIRS}
  )

  add_dependencies(${crt_name} sysroot-headers)

  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/sysroot/lib/${crt_name}.o
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SYSROOT_LIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_OBJECTS:${crt_name}> ${SYSROOT_LIB_DIR}/${crt_name}.o
    COMMAND_EXPAND_LISTS
  )

  add_custom_target(${crt_name}.o ALL
    DEPENDS ${CMAKE_BINARY_DIR}/sysroot/lib/${crt_name}.o)
  add_dependencies(${crt_name}.o ${crt_name})
endforeach()
