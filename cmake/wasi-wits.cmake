# Helper rules to download and vendor WITs used for WASI in this repository for
# bindings generation and future use.

# If `wkg` is on the system and has the right version, favor that,
# otherwise download a known good version.
find_program(WKG_EXECUTABLE NAMES wkg)
if(WKG_EXECUTABLE)
  message(STATUS "Found wkg: ${WKG_EXECUTABLE}")

  execute_process(
    COMMAND ${WKG_EXECUTABLE} --version
    OUTPUT_VARIABLE WKG_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (NOT (WKG_VERSION MATCHES "0\\.13\\.0"))
    message(WARNING "wkg version 0.13.0 is required, found: ${WKG_VERSION}")
    set(WKG_EXECUTABLE "")
  endif()
endif()

if (NOT WKG_EXECUTABLE)
  include(ba-download)
  ba_download(
    wkg
    "https://github.com/bytecodealliance/wasm-pkg-tools"
    "0.13.0"
  )
  ExternalProject_Get_Property(wkg DOWNLOADED_FILE)
  set(wkg_bin ${DOWNLOADED_FILE})
else()
  add_custom_target(wkg)
  set(wkg_bin ${WKG_EXECUTABLE})
endif()

foreach(version p2 p3)
  set(wit_dir ${CMAKE_SOURCE_DIR}/wasi/${version}/wit)
  set(stamp ${CMAKE_BINARY_DIR}/wasi${version}-wit.stamp)

  configure_file(${wit_dir}/wasi-libc.wit.in ${wit_dir}/wasi-libc.wit)
  add_custom_command(
    OUTPUT ${stamp}
    COMMAND ${wkg_bin} wit fetch
    COMMAND ${CMAKE_COMMAND} -E touch ${stamp}
    WORKING_DIRECTORY ${wit_dir}/..
    DEPENDS wkg ${wit_dir}/wasi-libc.wit
    COMMENT "Fetching WASI${version} dependencies"
  )
  add_custom_target(wasi${version}-wits DEPENDS ${stamp})
endforeach()
