/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * RMR Standalone Foundation V1 — pure core.
 * No libc. No allocator. No syscall. No filesystem. No clock.
 */
#include "../include/rmr_standalone.h"

void rmr_sa_zero(void *dst, rmr_sa_usize len) {
    rmr_sa_u8 *out = (rmr_sa_u8 *)dst;
    if (RMR_SA_UNLIKELY(out == (rmr_sa_u8 *)0)) {
        return;
    }
    for (rmr_sa_usize i = 0u; i < len; ++i) {
        out[i] = 0u;
    }
}

void rmr_sa_copy(void *dst, const void *src, rmr_sa_usize len) {
    rmr_sa_u8 *out = (rmr_sa_u8 *)dst;
    const rmr_sa_u8 *in = (const rmr_sa_u8 *)src;
    if (RMR_SA_UNLIKELY(out == (rmr_sa_u8 *)0 ||
                        in == (const rmr_sa_u8 *)0 ||
                        out == in)) {
        return;
    }
    for (rmr_sa_usize i = 0u; i < len; ++i) {
        out[i] = in[i];
    }
}

rmr_sa_u32 rmr_sa_equal(const void *left, const void *right, rmr_sa_usize len) {
    const rmr_sa_u8 *a = (const rmr_sa_u8 *)left;
    const rmr_sa_u8 *b = (const rmr_sa_u8 *)right;
    rmr_sa_u32 diff = 0u;

    if (RMR_SA_UNLIKELY((a == (const rmr_sa_u8 *)0 ||
                         b == (const rmr_sa_u8 *)0) && len != 0u)) {
        return 0u;
    }

    for (rmr_sa_usize i = 0u; i < len; ++i) {
        diff |= (rmr_sa_u32)(a[i] ^ b[i]);
    }
    return (rmr_sa_u32)(diff == 0u);
}

rmr_sa_u32 rmr_sa_xor_fold32(const void *data, rmr_sa_usize len) {
    const rmr_sa_u8 *in = (const rmr_sa_u8 *)data;
    rmr_sa_u32 fold = 0u;

    if (RMR_SA_UNLIKELY(in == (const rmr_sa_u8 *)0 && len != 0u)) {
        return 0u;
    }

    for (rmr_sa_usize i = 0u; i < len; ++i) {
        const rmr_sa_u32 shift = (rmr_sa_u32)((i & 3u) * 8u);
        fold ^= ((rmr_sa_u32)in[i]) << shift;
    }
    return fold;
}

rmr_sa_u32 rmr_sa_q_profile_get(rmr_sa_u32 fraction_bits,
                                rmr_sa_q_profile *out) {
    if (RMR_SA_UNLIKELY(out == (rmr_sa_q_profile *)0)) {
        return RMR_SA_ERR_ARGUMENT;
    }

    out->fraction_bits = fraction_bits;
    out->flags = 0u;

    switch (fraction_bits) {
        case RMR_SA_Q8_FRAC:
            out->storage_bits = 32u;
            out->arithmetic_bits = 32u;
            return RMR_SA_OK;
        case RMR_SA_Q16_FRAC:
            out->storage_bits = 32u;
            out->arithmetic_bits = 32u;
            return RMR_SA_OK;
        case RMR_SA_Q32_FRAC:
            out->storage_bits = 64u;
            out->arithmetic_bits = 64u;
            return RMR_SA_OK;
        case RMR_SA_Q42_FRAC:
            out->storage_bits = 64u;
            out->arithmetic_bits = 64u;
            return RMR_SA_OK;
        default:
            out->storage_bits = 0u;
            out->arithmetic_bits = 0u;
            return RMR_SA_ERR_UNSUPPORTED_Q;
    }
}
