include(ExternalProject)

function(ba_download target repo version)
  if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(arch "x86_64")
  elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(arch "aarch64")
  elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(arch "aarch64")
  elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "AMD64")
    set(arch "x86_64")
  else()
    message(FATAL_ERROR "Unsupported architecture ${CMAKE_HOST_SYSTEM_PROCESSOR} for ${target}")
  endif()

  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(os macos)
  elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(os linux)
  elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(os windows)
  else()
    message(FATAL_ERROR "Unsupported system ${CMAKE_HOST_SYSTEM_NAME} for ${target}")
  endif()

  if (target STREQUAL wasmtime)
    set(fmt tar.xz)
  elseif ((target STREQUAL wasm-component-ld) AND (os STREQUAL windows))
    set(fmt zip)
  else()
    set(fmt tar.gz)
  endif()

  if (target STREQUAL wit-bindgen)
    set(tag v${version})
  else()
    set(tag ${version})
  endif()

  message(STATUS "Using ${target} ${version} for ${arch}-${os} from ${repo}")

  ExternalProject_Add(
    ${target}
    EXCLUDE_FROM_ALL ON
    URL "${repo}/releases/download/${tag}/${target}-${version}-${arch}-${os}.${fmt}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
endfunction()
