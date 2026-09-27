/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * RMR Crypto Freestanding 140 — substrate V1.
 * No libc. No heap. No OS. No provider. No hidden native callback.
 */
#ifndef RMR_CF140_H
#define RMR_CF140_H

#if !defined(__GNUC__) && !defined(__clang__)
#error "RMR CF140 V1 requires GCC/Clang-compatible compiler built-in integer types"
#endif

typedef __UINT8_TYPE__  rmr_cf_u8;
typedef __UINT16_TYPE__ rmr_cf_u16;
typedef __UINT32_TYPE__ rmr_cf_u32;
typedef __UINT64_TYPE__ rmr_cf_u64;
typedef __SIZE_TYPE__   rmr_cf_usize;

#if defined(__GNUC__) || defined(__clang__)
#define RMR_CF_INLINE static inline __attribute__((always_inline))
#define RMR_CF_NOINLINE __attribute__((noinline))
#define RMR_CF_USED __attribute__((used))
#define RMR_CF_HIDDEN __attribute__((visibility("hidden")))
#define RMR_CF_SECTION(name) __attribute__((section(name)))
#else
#define RMR_CF_INLINE static inline
#define RMR_CF_NOINLINE
#define RMR_CF_USED
#define RMR_CF_HIDDEN
#define RMR_CF_SECTION(name)
#endif

#define RMR_CF_OK 0u
#define RMR_CF_VERSION 1u

RMR_CF_INLINE rmr_cf_u32 rmr_cf_xor32(rmr_cf_u32 a, rmr_cf_u32 b) {
    return a ^ b;
}

RMR_CF_INLINE rmr_cf_u64 rmr_cf_xor64(rmr_cf_u64 a, rmr_cf_u64 b) {
    return a ^ b;
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_and32(rmr_cf_u32 a, rmr_cf_u32 b) {
    return a & b;
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_or32(rmr_cf_u32 a, rmr_cf_u32 b) {
    return a | b;
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_not32(rmr_cf_u32 a) {
    return ~a;
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_add32(rmr_cf_u32 a, rmr_cf_u32 b) {
    return a + b;
}

RMR_CF_INLINE rmr_cf_u64 rmr_cf_add64(rmr_cf_u64 a, rmr_cf_u64 b) {
    return a + b;
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_rotl32(rmr_cf_u32 x, rmr_cf_u32 n) {
    const rmr_cf_u32 s = n & 31u;
    return (x << s) | (x >> ((32u - s) & 31u));
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_rotr32(rmr_cf_u32 x, rmr_cf_u32 n) {
    const rmr_cf_u32 s = n & 31u;
    return (x >> s) | (x << ((32u - s) & 31u));
}

RMR_CF_INLINE rmr_cf_u64 rmr_cf_rotl64(rmr_cf_u64 x, rmr_cf_u32 n) {
    const rmr_cf_u32 s = n & 63u;
    return (x << s) | (x >> ((64u - s) & 63u));
}

RMR_CF_INLINE rmr_cf_u64 rmr_cf_rotr64(rmr_cf_u64 x, rmr_cf_u32 n) {
    const rmr_cf_u32 s = n & 63u;
    return (x >> s) | (x << ((64u - s) & 63u));
}

RMR_CF_INLINE rmr_cf_u32 rmr_cf_select32(rmr_cf_u32 mask,
                                         rmr_cf_u32 when_one,
                                         rmr_cf_u32 when_zero) {
    return (mask & when_one) | (~mask & when_zero);
}

RMR_CF_INLINE rmr_cf_u64 rmr_cf_select64(rmr_cf_u64 mask,
                                         rmr_cf_u64 when_one,
                                         rmr_cf_u64 when_zero) {
    return (mask & when_one) | (~mask & when_zero);
}

rmr_cf_u32 rmr_cf_load32_le(const rmr_cf_u8 src[4]);
rmr_cf_u64 rmr_cf_load64_le(const rmr_cf_u8 src[8]);
rmr_cf_u32 rmr_cf_load32_be(const rmr_cf_u8 src[4]);
void rmr_cf_store32_le(rmr_cf_u8 dst[4], rmr_cf_u32 value);
void rmr_cf_store64_le(rmr_cf_u8 dst[8], rmr_cf_u64 value);
void rmr_cf_store32_be(rmr_cf_u8 dst[4], rmr_cf_u32 value);

void rmr_cf_xor16(rmr_cf_u8 dst[16],
                  const rmr_cf_u8 left[16],
                  const rmr_cf_u8 right[16]);
void rmr_cf_xor32_bytes(rmr_cf_u8 dst[32],
                        const rmr_cf_u8 left[32],
                        const rmr_cf_u8 right[32]);
void rmr_cf_xor64_bytes(rmr_cf_u8 dst[64],
                        const rmr_cf_u8 left[64],
                        const rmr_cf_u8 right[64]);

rmr_cf_u32 rmr_cf_diff16(const rmr_cf_u8 left[16],
                         const rmr_cf_u8 right[16]);
rmr_cf_u32 rmr_cf_diff32(const rmr_cf_u8 left[32],
                         const rmr_cf_u8 right[32]);

#endif
