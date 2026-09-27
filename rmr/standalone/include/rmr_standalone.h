/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * RMR Standalone Foundation V1
 * Pure freestanding boundary: no libc, no heap, no OS, no filesystem.
 */
#ifndef RMR_STANDALONE_H
#define RMR_STANDALONE_H

#if !defined(__GNUC__) && !defined(__clang__)
#error "RMR standalone V1 requires GCC/Clang-compatible compiler built-in types"
#endif

typedef __UINT8_TYPE__  rmr_sa_u8;
typedef __UINT16_TYPE__ rmr_sa_u16;
typedef __UINT32_TYPE__ rmr_sa_u32;
typedef __UINT64_TYPE__ rmr_sa_u64;
typedef __INT32_TYPE__  rmr_sa_i32;
typedef __INT64_TYPE__  rmr_sa_i64;
typedef __SIZE_TYPE__   rmr_sa_usize;

#define RMR_SA_VERSION 1u

#if defined(__GNUC__) || defined(__clang__)
#define RMR_SA_INLINE static inline __attribute__((always_inline))
#define RMR_SA_NOINLINE __attribute__((noinline))
#define RMR_SA_USED __attribute__((used))
#define RMR_SA_HIDDEN __attribute__((visibility("hidden")))
#define RMR_SA_SECTION(name) __attribute__((section(name)))
#define RMR_SA_LIKELY(x) __builtin_expect(!!(x), 1)
#define RMR_SA_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define RMR_SA_INLINE static inline
#define RMR_SA_NOINLINE
#define RMR_SA_USED
#define RMR_SA_HIDDEN
#define RMR_SA_SECTION(name)
#define RMR_SA_LIKELY(x) (x)
#define RMR_SA_UNLIKELY(x) (x)
#endif

#define RMR_SA_OK                 0u
#define RMR_SA_ERR_ARGUMENT       1u
#define RMR_SA_ERR_UNSUPPORTED_Q  2u

#define RMR_SA_Q8_FRAC   8u
#define RMR_SA_Q16_FRAC 16u
#define RMR_SA_Q32_FRAC 32u
#define RMR_SA_Q42_FRAC 42u

typedef struct rmr_sa_q_profile {
    rmr_sa_u32 fraction_bits;
    rmr_sa_u32 storage_bits;
    rmr_sa_u32 arithmetic_bits;
    rmr_sa_u32 flags;
} rmr_sa_q_profile;

void rmr_sa_zero(void *dst, rmr_sa_usize len);
void rmr_sa_copy(void *dst, const void *src, rmr_sa_usize len);

/*
 * Full-scan byte equality. It avoids an early return on byte mismatch.
 * V1 makes no constant-time timing claim across compiler/ISA/microarchitecture.
 */
rmr_sa_u32 rmr_sa_equal(const void *left, const void *right, rmr_sa_usize len);

/*
 * Non-cryptographic deterministic fold for probes/fixtures only.
 * Never use as a replacement for BLAKE3/SHA-256/MAC/signature.
 */
rmr_sa_u32 rmr_sa_xor_fold32(const void *data, rmr_sa_usize len);

/*
 * Q profiles are numeric representation contracts, not security levels.
 */
rmr_sa_u32 rmr_sa_q_profile_get(rmr_sa_u32 fraction_bits,
                                rmr_sa_q_profile *out);

#endif
