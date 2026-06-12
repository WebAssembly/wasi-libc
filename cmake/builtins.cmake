#
# Figure out what to do about compiler-rt.
#
# The compiler-rt library is not built here in the wasi-libc repository, but it
# is required to link artifacts. Notably `libc.so` and test and such all require
# it to exist. Currently the ways this is handled are:
#
# * If `BUILTINS_LIB` is defined at build time then that's assumed to be a path
#   to the libcompiler-rt.a. That's then ingested into the build here and copied
#   around to special locations to get the `*.so` rules below to work (see docs
#   there).
#
# * If `BUILTINS_LIB` is not defined then a known-good copy is downloaded from
#   wasi-sdk CI and used instead.
#
# In the future this may also want some form of configuration to support
# assuming the system compiler has a compiler-rt, e.g. if $(SYSTEM_BUILTINS_LIB)
# exists that should be used instead.
get_property(directory_cflags DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY COMPILE_OPTIONS)
execute_process(
  COMMAND ${CMAKE_C_COMPILER} --target=${TARGET_TRIPLE} --print-libgcc-file-name
  OUTPUT_VARIABLE system_builtins_lib
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND ${CMAKE_C_COMPILER} --target=${TARGET_TRIPLE} -print-resource-dir
  OUTPUT_VARIABLE system_resource_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
cmake_path(
  RELATIVE_PATH system_builtins_lib
  BASE_DIRECTORY ${system_resource_dir}
  OUTPUT_VARIABLE builtins_lib_rel_1
)


set(tmp_resource_dir ${CMAKE_BINARY_DIR}/temp-resource-dir)
set(builtins_lib_path ${tmp_resource_dir}/${builtins_lib_rel_1})
cmake_path(GET builtins_lib_path PARENT_PATH builtins_lib_dir)

if(BUILTINS_LIB)
  message(STATUS "Using builtins lib: ${BUILTINS_LIB}")
  set(builtins_lib_src ${BUILTINS_LIB})
  set(builtins_lib_dep ${BUILTINS_LIB})
else()
  message(STATUS "Using historical builtins lib from wasi-sdk...")
  include(ExternalProject)
  ExternalProject_Add(
    wasi-sdk-builtins
    EXCLUDE_FROM_ALL ON
    URL https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-25/libclang_rt.builtins-wasm32-wasi-25.0.tar.gz
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  ExternalProject_Get_Property(wasi-sdk-builtins SOURCE_DIR)
  set(builtins_lib_src ${SOURCE_DIR}/libclang_rt.builtins-wasm32.a)
  set(builtins_lib_dep wasi-sdk-builtins)
endif()

add_custom_command(
  OUTPUT ${builtins_lib_path}

  # Copy `libclang_rt.*` into place from the source into the final destination
  # within `tmp_resource_dir`. This is needed during linking to find
  # `libclang_rt.*` under the appropriate path for this target.
  COMMAND ${CMAKE_COMMAND} -E copy ${builtins_lib_src} ${builtins_lib_path}

  # Additionally fill in the `include` directory for the `tmp_resource_dir` that
  # we're creating. This is required to get access to the compiler's `stddef.h`,
  # for example. Here we symlink to whatever the compiler already has as that's
  # as good as we're going to get.
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${system_resource_dir}/include ${tmp_resource_dir}/include

  DEPENDS ${builtins_lib_dep}
)
add_custom_target(builtins DEPENDS ${builtins_lib_path})
