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
    set(arch "UNKNOWN_ARCH")
    message(WARNING "Unsupported architecture ${CMAKE_HOST_SYSTEM_PROCESSOR} for ${target}")
  endif()

  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    if (target STREQUAL wkg)
      set(os apple-darwin)
    else()
      set(os macos)
    endif()
  elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    if (target STREQUAL wkg)
      set(os unknown-linux-gnu)
    else()
      set(os linux)
    endif()
  elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    if (target STREQUAL wkg)
      set(os pc-windows-gnu)
    else()  
      set(os windows)
    endif()
  else()
    set(os "UNKNOWN_OS")
    message(WARNING "Unsupported system ${CMAKE_HOST_SYSTEM_NAME} for ${target}")
  endif()

  if (target STREQUAL wasmtime)
    set(fmt tar.xz)
  elseif ((os STREQUAL windows) AND
      ((target STREQUAL wasm-component-ld) OR (target STREQUAL wasm-tools)))
    set(fmt zip)
  else()
    set(fmt tar.gz)
  endif()

  if (target STREQUAL wit-bindgen OR target STREQUAL wasm-tools OR target STREQUAL wkg)
    set(tag v${version})
  else()
    set(tag ${version})
  endif()


  message(STATUS "Using ${target} ${version} for ${arch}-${os} from ${repo}")

  if (target STREQUAL wkg)
    # wkg ships a single binary rather than an archive
    set(download_path "${CMAKE_CURRENT_BINARY_DIR}/${target}")
    set(url "${repo}/releases/download/${tag}/${target}-${arch}-${os}")
    
    if (NOT EXISTS "${download_path}")
      message(STATUS "Downloading ${target} from ${url}")
      file(DOWNLOAD "${url}" "${download_path}" STATUS download_status)
      list(GET download_status 0 status_code)
      if (NOT status_code EQUAL 0)
        list(GET download_status 1 error_message)
        message(FATAL_ERROR "Failed to download ${target}: ${error_message}")
      endif()
      
      if (NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        execute_process(COMMAND chmod +x "${download_path}")
      endif()
    endif()
    
    add_custom_target(${target})
    set(${target}_bin "${download_path}" PARENT_SCOPE)
  else()
    ExternalProject_Add(
      ${target}
      EXCLUDE_FROM_ALL ON
      URL "${repo}/releases/download/${tag}/${target}-${version}-${arch}-${os}.${fmt}"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
    )
  endif()
endfunction()
