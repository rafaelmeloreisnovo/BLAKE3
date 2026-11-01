cmake_minimum_required(VERSION 3.22.1)
project(rmr_zipraf_free C)
get_filename_component(RMR_REPO "\${CMAKE_CURRENT_LIST_DIR}/../../../../../.." ABSOLUTE)
add_library(rmrfree SHARED
    zipraf_free.c
    "\${RMR_REPO}/c/blake3.c"
    "\${RMR_REPO}/c/blake3_dispatch.c"
    "\${RMR_REPO}/c/blake3_portable.c"
)
target_include_directories(rmrfree PRIVATE "\${RMR_REPO}/c")
target_compile_definitions(rmrfree PRIVATE
    BLAKE3_NO_SSE2 BLAKE3_NO_SSE41 BLAKE3_NO_AVX2 BLAKE3_NO_AVX512
    BLAKE3_USE_NEON=0 BLAKE3_ATOMICS=0
)
target_compile_options(rmrfree PRIVATE
    -std=c11 -Oz -Wall -Wextra -Werror
    -ffunction-sections -fdata-sections -fvisibility=hidden
    -ffreestanding -fno-builtin -fno-stack-protector
)
target_link_options(rmrfree PRIVATE -Wl,--gc-sections -Wl,--strip-all)
# JNI is Android hosted; core functions are caller-owned and no-libc.
