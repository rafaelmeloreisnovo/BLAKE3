/* Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR. */
#include "../include/rmr_fc16.h"

#if defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
#elif (defined(__x86_64__) || defined(__i386__)) && defined(__SSE4_2__)
#include <nmmintrin.h>
#endif

#if !((defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)) || ((defined(__x86_64__) || defined(__i386__)) && defined(__SSE4_2__)))
static const rmr_fc16_u32 crc32c_nibble[16] = {
    0x00000000u,0x105ec76fu,0x20bd8edeu,0x30e349b1u,
    0x417b1dbcu,0x5125dad3u,0x61c69362u,0x7198540du,
    0x82f63b78u,0x92a8fc17u,0xa24bb5a6u,0xb21572c9u,
    0xc38d26c4u,0xd3d3e1abu,0xe330a81au,0xf36e6f75u
};
#endif

static const rmr_fc16_u32 crc32_nibble[16] = {
    0x00000000u,0x1db71064u,0x3b6e20c8u,0x26d930acu,
    0x76dc4190u,0x6b6b51f4u,0x4db26158u,0x5005713cu,
    0xedb88320u,0xf00f9344u,0xd6d6a3e8u,0xcb61b38cu,
    0x9b64c2b0u,0x86d3d2d4u,0xa00ae278u,0xbdbdf21cu
};

#if (defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)) || ((defined(__x86_64__) || defined(__i386__)) && defined(__SSE4_2__))
static rmr_fc16_u64 load_le64(const rmr_fc16_u8 *p) {
    rmr_fc16_u64 v = 0u;
    for (rmr_fc16_u32 i = 0; i < 8u; ++i) v |= ((rmr_fc16_u64)p[i]) << (i * 8u);
    return v;
}

static rmr_fc16_u32 load_le32(const rmr_fc16_u8 *p) {
    return ((rmr_fc16_u32)p[0]) |
           ((rmr_fc16_u32)p[1] << 8u) |
           ((rmr_fc16_u32)p[2] << 16u) |
           ((rmr_fc16_u32)p[3] << 24u);
}
#endif

rmr_fc16_u32 rmr_fc16_crc32c_backend_flags(void) {
#if defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)
    return RMR_FC16_FLAG_CRC32C_HW_ARM;
#elif (defined(__x86_64__) || defined(__i386__)) && defined(__SSE4_2__)
    return RMR_FC16_FLAG_CRC32C_HW_X86;
#else
    return 0u;
#endif
}

rmr_fc16_u32 rmr_fc16_crc32c_update(rmr_fc16_u32 state,
                                    const rmr_fc16_u8 *data,
                                    rmr_fc16_usize len) {
    if (data == (const rmr_fc16_u8 *)0) return state;
    rmr_fc16_usize i = 0u;
#if defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)
    for (; i + 8u <= len; i += 8u) state = __crc32cd(state, load_le64(data + i));
    for (; i + 4u <= len; i += 4u) state = __crc32cw(state, load_le32(data + i));
    for (; i < len; ++i) state = __crc32cb(state, data[i]);
#elif (defined(__x86_64__) || defined(__i386__)) && defined(__SSE4_2__)
#if defined(__x86_64__)
    for (; i + 8u <= len; i += 8u) state = (rmr_fc16_u32)_mm_crc32_u64(state, load_le64(data + i));
#endif
    for (; i + 4u <= len; i += 4u) state = _mm_crc32_u32(state, load_le32(data + i));
    for (; i < len; ++i) state = _mm_crc32_u8(state, data[i]);
#else
    for (; i < len; ++i) {
        state ^= data[i];
        state = (state >> 4u) ^ crc32c_nibble[state & 0x0fu];
        state = (state >> 4u) ^ crc32c_nibble[state & 0x0fu];
    }
#endif
    return state;
}

rmr_fc16_u32 rmr_fc16_crc32c_finish(rmr_fc16_u32 state) { return state ^ 0xffffffffu; }

rmr_fc16_u32 rmr_fc16_crc32_ieee(const rmr_fc16_u8 *data, rmr_fc16_usize len) {
    rmr_fc16_u32 state = 0xffffffffu;
    if (data == (const rmr_fc16_u8 *)0) return 0u;
    for (rmr_fc16_usize i = 0u; i < len; ++i) {
        state ^= data[i];
        state = (state >> 4u) ^ crc32_nibble[state & 0x0fu];
        state = (state >> 4u) ^ crc32_nibble[state & 0x0fu];
    }
    return state ^ 0xffffffffu;
}
