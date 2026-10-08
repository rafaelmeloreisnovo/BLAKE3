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
