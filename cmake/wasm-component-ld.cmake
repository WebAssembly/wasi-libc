option(USE_WASM_COMPONENT_LD "Use wasm-component-ld as the linker for WebAssembly components" ON)
if (NOT USE_WASM_COMPONENT_LD)
  return()
endif()

find_program(WASM_COMPONENT_LD_EXECUTABLE NAMES wasm-component-ld)

if (NOT WASM_COMPONENT_LD_EXECUTABLE)
  include(ba-download)
  ba_download(
    wasm-component-ld
    "https://github.com/bytecodealliance/wasm-component-ld"
    "v0.5.19"
  )
  ExternalProject_Get_Property(wasm-component-ld SOURCE_DIR)
  set(WASM_COMPONENT_LD_EXECUTABLE "${SOURCE_DIR}/wasm-component-ld")
  add_dependencies(sysroot_inc wasm-component-ld)
endif()

add_link_options(-fuse-ld=${WASM_COMPONENT_LD_EXECUTABLE})
