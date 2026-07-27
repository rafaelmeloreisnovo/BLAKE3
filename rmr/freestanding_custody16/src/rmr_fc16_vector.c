/* Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR. */
#include "../include/rmr_fc16.h"

#if defined(__aarch64__) || defined(__ARM_NEON)
#include <arm_neon.h>
#elif defined(__SSE2__)
#include <emmintrin.h>
#endif

rmr_fc16_u32 rmr_fc16_vector_backend_flags(void) {
#if defined(__aarch64__) || defined(__ARM_NEON)
    return RMR_FC16_FLAG_SIMD_NEON;
#elif defined(__SSE2__)
    return RMR_FC16_FLAG_SIMD_SSE2;
#else
    return 0u;
#endif
}

rmr_fc16_u32 rmr_fc16_diff16(const rmr_fc16_u8 before[16],
                             const rmr_fc16_u8 after[16],
                             rmr_fc16_u8 xor_mask[16]) {
    if (!before || !after || !xor_mask) return 0u;
#if defined(__aarch64__) || defined(__ARM_NEON)
    const uint8x16_t a = vld1q_u8(before);
    const uint8x16_t b = vld1q_u8(after);
    const uint8x16_t d = veorq_u8(a, b);
    vst1q_u8(xor_mask, d);
#elif defined(__SSE2__)
    const __m128i a = _mm_loadu_si128((const __m128i *)(const void *)before);
    const __m128i b = _mm_loadu_si128((const __m128i *)(const void *)after);
    _mm_storeu_si128((__m128i *)(void *)xor_mask, _mm_xor_si128(a, b));
#else
    for (rmr_fc16_u32 i = 0u; i < 16u; ++i) xor_mask[i] = before[i] ^ after[i];
#endif
    rmr_fc16_u32 changed = 0u;
    for (rmr_fc16_u32 i = 0u; i < 16u; ++i) changed |= xor_mask[i];
    return changed != 0u;
}

void rmr_fc16_apply16(rmr_fc16_u8 target[16], const rmr_fc16_u8 xor_mask[16]) {
    if (!target || !xor_mask) return;
#if defined(__aarch64__) || defined(__ARM_NEON)
    const uint8x16_t a = vld1q_u8(target);
    const uint8x16_t m = vld1q_u8(xor_mask);
    vst1q_u8(target, veorq_u8(a, m));
#elif defined(__SSE2__)
    const __m128i a = _mm_loadu_si128((const __m128i *)(const void *)target);
    const __m128i m = _mm_loadu_si128((const __m128i *)(const void *)xor_mask);
    _mm_storeu_si128((__m128i *)(void *)target, _mm_xor_si128(a, m));
#else
    for (rmr_fc16_u32 i = 0u; i < 16u; ++i) target[i] ^= xor_mask[i];
#endif
}

rmr_fc16_u32 rmr_fc16_xor_fold16(const rmr_fc16_u8 data[16]) {
    if (!data) return 0u;
    rmr_fc16_u32 v = 0u;
    for (rmr_fc16_u32 i = 0u; i < 16u; ++i) v ^= ((rmr_fc16_u32)data[i]) << ((i & 3u) * 8u);
    return v;
}
