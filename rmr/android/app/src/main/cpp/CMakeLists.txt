cmake_minimum_required(VERSION 3.22.1)
project(rmr_android_blake3 C)

# CMakeLists.txt lives at rmr/android/app/src/main/cpp/.
get_filename_component(BLAKE3_REPO "${CMAKE_CURRENT_LIST_DIR}/../../../../../.." ABSOLUTE)
set(RMR_PORTABLE "${BLAKE3_REPO}/rmr/portable")

add_library(rmr_provider STATIC
    "${RMR_PORTABLE}/provider/blake3/rmr_portable_blake3_v1.c"
    "${RMR_PORTABLE}/provider/blake3/rmr_portable_memshim.c"
    "${BLAKE3_REPO}/c/blake3.c"
    "${BLAKE3_REPO}/c/blake3_dispatch.c"
    "${BLAKE3_REPO}/c/blake3_portable.c"
)
target_include_directories(rmr_provider PRIVATE
    "${RMR_PORTABLE}/include"
    "${RMR_PORTABLE}/provider/blake3/include"
    "${BLAKE3_REPO}/c"
)
target_compile_definitions(rmr_provider PRIVATE
    BLAKE3_NO_SSE2 BLAKE3_NO_SSE41 BLAKE3_NO_AVX2 BLAKE3_NO_AVX512
    BLAKE3_USE_NEON=0 BLAKE3_ATOMICS=0 NDEBUG
)
target_compile_options(rmr_provider PRIVATE
    -O2 -ffreestanding -fno-builtin -fno-stack-protector
    -fvisibility=hidden -ffunction-sections -fdata-sections
)
set_target_properties(rmr_provider PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(rmrhash SHARED rmr_jni_bridge.c)
target_include_directories(rmrhash PRIVATE "${RMR_PORTABLE}/include")
target_compile_options(rmrhash PRIVATE -O2 -Wall -Wextra -Werror -fvisibility=hidden)
target_link_libraries(rmrhash PRIVATE rmr_provider)
target_link_options(rmrhash PRIVATE
    -Wl,--gc-sections -Wl,--exclude-libs,ALL
)

# Three independently loadable Android .so files:
# - officialhash: exact public BLAKE3-team C checkout, pinned by CI
# - forkhash: THIS fork's root c/ sources
# - rmrhash: existing RMR portable adapter + THIS fork's c/ source (unchanged)
# Libraries are isolated; identical digest semantics are required, not a
# purported third cryptographic algorithm.
if(NOT DEFINED ENV{BLAKE3_OFFICIAL_DIR})
    message(FATAL_ERROR "BLAKE3_OFFICIAL_DIR must point at a verified pinned upstream checkout")
endif()
get_filename_component(BLAKE3_OFFICIAL "$ENV{BLAKE3_OFFICIAL_DIR}" ABSOLUTE)
foreach(source_file blake3.c blake3_dispatch.c blake3_portable.c blake3.h blake3_impl.h)
    if(NOT EXISTS "${BLAKE3_OFFICIAL}/c/${source_file}")
        message(FATAL_ERROR "Missing official source file: ${source_file}")
    endif()
endforeach()

function(add_comparison_provider name code_root define_name)
    add_library(${name} SHARED
        b3_compare_jni.c
        "${code_root}/blake3.c"
        "${code_root}/blake3_dispatch.c"
        "${code_root}/blake3_portable.c"
    )
    target_include_directories(${name} PRIVATE "${code_root}")
    target_compile_definitions(${name} PRIVATE
        ${define_name}
        BLAKE3_NO_SSE2 BLAKE3_NO_SSE41 BLAKE3_NO_AVX2 BLAKE3_NO_AVX512
        BLAKE3_USE_NEON=0 BLAKE3_ATOMICS=0
    )
    target_compile_options(${name} PRIVATE
        -std=c11 -O2 -Wall -Wextra -fvisibility=hidden
        -ffunction-sections -fdata-sections
    )
    target_link_options(${name} PRIVATE -Wl,--gc-sections)
endfunction()

add_comparison_provider(officialhash "${BLAKE3_OFFICIAL}/c" RMR_COMPARE_OFFICIAL)
add_comparison_provider(forkhash "${BLAKE3_REPO}/c" RMR_COMPARE_FORK)
