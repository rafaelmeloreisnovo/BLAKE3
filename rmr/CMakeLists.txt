# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(rmr_external
  VERSION 1.0
  DESCRIPTION "RMR external execution, custody and benchmark layer"
  LANGUAGES C CXX ASM
)

option(RMR_BUILD_TESTS "Build RMR host self-tests" ON)
option(RMR_BUILD_TOPOLOGY "Build RMR hardware/build topology micro-module" ON)
option(RMR_BUILD_HWIF "Build RMR hardware interface library" ON)
option(RMR_ARMV7_PRIVILEGED "Enable privileged ARMv7 CP15 HWIF leaf" OFF)
option(RMR_STRICT_WARNINGS "Enable stricter warnings for new RMR topology code" ON)
option(RMR_BLAKE3_USE_TBB "Build repository BLAKE3 with optional oneTBB path" OFF)

set(BLAKE3_USE_TBB "${RMR_BLAKE3_USE_TBB}" CACHE BOOL "" FORCE)
set(BLAKE3_FETCH_TBB OFF CACHE BOOL "" FORCE)
set(BLAKE3_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BLAKE3_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../c" "${CMAKE_BINARY_DIR}/blake3-c")

set(RMR_PAI_SOURCES
  core/main.c
  core/cli.c
  core/base.c
  core/geom.c
  core/toroid.c
  core/hash_sha256.c
  core/hash_blake3.c
  core/scan.c
  core/sign.c
  core/bench.c
  core/benchdiff.c
  core/validate.c
  core/util.c
  ui/mode_router.c
  ui/bbs/menu.c
  ui/bbs/input.c
  ui/bbs/screen.c
  pathcutter/pathcutter.c
  hwif/detect/detect_fallback.c
  hwif/detect/detect_x86.c
  hwif/detect/detect_aarch64.c
)

add_executable(pai ${RMR_PAI_SOURCES})
target_compile_features(pai PRIVATE c_std_11)
target_compile_definitions(pai PRIVATE _GNU_SOURCE)
target_include_directories(pai PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/core"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/hwif/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)
target_link_libraries(pai PRIVATE BLAKE3::blake3)
if(UNIX)
  target_link_libraries(pai PRIVATE m)
endif()
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  target_compile_options(pai PRIVATE -Wall -Wextra)
endif()


if(RMR_BUILD_HWIF)
  set(RMR_HWIF_SOURCES
    hwif/rmr_hwif.c
  )

  if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    list(APPEND RMR_HWIF_SOURCES hwif/asm/x86_64/rmr_hwif_backend.S)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64|ARM64)$")
    list(APPEND RMR_HWIF_SOURCES hwif/asm/aarch64/rmr_hwif_backend.S)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv7|armv7l)")
    list(APPEND RMR_HWIF_SOURCES hwif/asm/armv7/rmr_hwif_user.S)
    if(RMR_ARMV7_PRIVILEGED)
      list(APPEND RMR_HWIF_SOURCES hwif/asm/armv7/rmr_hwif_privileged.S)
    endif()
  endif()

  add_library(rmr-hwif STATIC ${RMR_HWIF_SOURCES})
  target_compile_features(rmr-hwif PUBLIC c_std_11)
  target_include_directories(rmr-hwif PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/hwif/include"
  )

  if(RMR_ARMV7_PRIVILEGED)
    target_compile_definitions(rmr-hwif PRIVATE RMR_ARMV7_ASSUME_PRIVILEGED=1)
  endif()

  if(RMR_STRICT_WARNINGS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(rmr-hwif PRIVATE
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wpointer-arith
      -Wcast-align
      -Wstrict-prototypes
      -Wmissing-prototypes
      -Wformat=2
      -Wundef
      -Wwrite-strings
    )
  endif()
endif()

if(RMR_BUILD_TOPOLOGY)
  add_library(rmr-topology STATIC
    topology/src/rmr_topology.c
  )
  target_compile_features(rmr-topology PUBLIC c_std_11)
  target_include_directories(rmr-topology PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/topology/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )
  if(RMR_STRICT_WARNINGS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(rmr-topology PRIVATE
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wpointer-arith
      -Wcast-align
      -Wstrict-prototypes
      -Wmissing-prototypes
      -Wformat=2
      -Wundef
      -Wwrite-strings
    )
  endif()
endif()

add_executable(rmr-blake3-bench benchmark_framework/core/blake3_bench.c)
target_compile_features(rmr-blake3-bench PRIVATE c_std_11)
target_link_libraries(rmr-blake3-bench PRIVATE BLAKE3::blake3)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  target_compile_options(rmr-blake3-bench PRIVATE -O3 -Wall -Wextra -Wpedantic)
endif()

if(RMR_BUILD_TESTS)
  include(CTest)
  add_executable(rmr-blake3-backend-selftest
    tests/rmr_blake3_backend_selftest.c
    core/hash_blake3.c
  )
  target_compile_features(rmr-blake3-backend-selftest PRIVATE c_std_11)
  target_include_directories(rmr-blake3-backend-selftest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
  )
  target_link_libraries(rmr-blake3-backend-selftest PRIVATE BLAKE3::blake3)
  add_test(NAME rmr_blake3_backend_selftest COMMAND rmr-blake3-backend-selftest)

  if(RMR_BUILD_HWIF)
    add_executable(rmr-hwif-selftest
      hwif/tests/rmr_hwif_selftest.c
    )
    target_compile_features(rmr-hwif-selftest PRIVATE c_std_11)
    target_link_libraries(rmr-hwif-selftest PRIVATE rmr-hwif)
    add_test(NAME rmr_hwif_selftest COMMAND rmr-hwif-selftest)
  endif()

  if(RMR_BUILD_TOPOLOGY)
    add_executable(rmr-topology-selftest
      topology/tests/rmr_topology_selftest.c
    )
    target_compile_features(rmr-topology-selftest PRIVATE c_std_11)
    target_link_libraries(rmr-topology-selftest PRIVATE rmr-topology)
    add_test(NAME rmr_topology_selftest COMMAND rmr-topology-selftest)
  endif()
endif()
