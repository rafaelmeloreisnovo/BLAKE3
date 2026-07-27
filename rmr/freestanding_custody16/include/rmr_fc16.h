/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Freestanding Custody16: fixed-memory, no-libc, no-heap deterministic core.
 */
#ifndef RMR_FC16_H
#define RMR_FC16_H

#if !defined(__GNUC__) && !defined(__clang__)
#error "Custody16 requires a GCC/Clang-compatible freestanding compiler"
#endif

typedef __UINT8_TYPE__  rmr_fc16_u8;
typedef __UINT16_TYPE__ rmr_fc16_u16;
typedef __UINT32_TYPE__ rmr_fc16_u32;
typedef __UINT64_TYPE__ rmr_fc16_u64;
typedef __SIZE_TYPE__   rmr_fc16_usize;

#define RMR_FC16_VERSION             1u
#define RMR_FC16_LANES              16u
#define RMR_FC16_MATRIX_SIDE         4u
#define RMR_FC16_SHA256_BYTES       32u
#define RMR_FC16_SEAL_BYTES        512u
#define RMR_FC16_ZIP_OVERHEAD       128u
#define RMR_FC16_MAX_BENCH_SAMPLES  31u

#define RMR_FC16_OK                    0u
#define RMR_FC16_ERR_ARGUMENT          1u
#define RMR_FC16_ERR_STATE             2u
#define RMR_FC16_ERR_CAPACITY          3u
#define RMR_FC16_ERR_SIZE              4u
#define RMR_FC16_ERR_INTEGRITY         5u
#define RMR_FC16_ERR_THERMAL           6u

#define RMR_FC16_FLAG_CRC32C_HW_ARM   (1u << 0)
#define RMR_FC16_FLAG_CRC32C_HW_X86   (1u << 1)
#define RMR_FC16_FLAG_SIMD_NEON       (1u << 2)
#define RMR_FC16_FLAG_SIMD_SSE2       (1u << 3)
#define RMR_FC16_FLAG_THERMAL_ACCEPT  (1u << 4)
#define RMR_FC16_FLAG_SNAPSHOT_MATCH  (1u << 5)
#define RMR_FC16_FLAG_HW_STAMP_VALID  (1u << 6)

#define RMR_FC16_ENTRY_REGULAR         1u
#define RMR_FC16_ENTRY_DIRECTORY       2u
#define RMR_FC16_ENTRY_SYMLINK_SKIPPED 3u

/* Running CRC32C state: initialize with 0xffffffff and finalize with xor. */
rmr_fc16_u32 rmr_fc16_crc32c_update(rmr_fc16_u32 state,
                                    const rmr_fc16_u8 *data,
                                    rmr_fc16_usize len);
rmr_fc16_u32 rmr_fc16_crc32c_finish(rmr_fc16_u32 state);
rmr_fc16_u32 rmr_fc16_crc32c_backend_flags(void);

/* IEEE CRC-32 used by ZIP, separate from CRC32C/Castagnoli. */
rmr_fc16_u32 rmr_fc16_crc32_ieee(const rmr_fc16_u8 *data,
                                 rmr_fc16_usize len);

/* Toolchain intrinsics only; no runtime library dependency. */
rmr_fc16_u32 rmr_fc16_vector_backend_flags(void);
rmr_fc16_u32 rmr_fc16_diff16(const rmr_fc16_u8 before[16],
                             const rmr_fc16_u8 after[16],
                             rmr_fc16_u8 xor_mask[16]);
void rmr_fc16_apply16(rmr_fc16_u8 target[16],
                      const rmr_fc16_u8 xor_mask[16]);
rmr_fc16_u32 rmr_fc16_xor_fold16(const rmr_fc16_u8 data[16]);

/* Internal SHA-256, used to cryptographically seal the canonical capsule. */
typedef struct rmr_fc16_sha256_ctx {
    rmr_fc16_u32 h[8];
    rmr_fc16_u64 bit_len;
    rmr_fc16_u8 buffer[64];
    rmr_fc16_u32 buffer_len;
} rmr_fc16_sha256_ctx;

void rmr_fc16_sha256_init(rmr_fc16_sha256_ctx *ctx);
void rmr_fc16_sha256_update(rmr_fc16_sha256_ctx *ctx,
                            const rmr_fc16_u8 *data,
                            rmr_fc16_usize len);
void rmr_fc16_sha256_final(rmr_fc16_sha256_ctx *ctx,
                           rmr_fc16_u8 out[RMR_FC16_SHA256_BYTES]);

/* One deterministic 4x4 lane matrix. Each lane has a fixed reduction slot. */
typedef struct rmr_fc16_lane {
    rmr_fc16_u32 crc_state;
    rmr_fc16_u32 xor_fold;
    rmr_fc16_u64 bytes;
    rmr_fc16_u64 blocks16;
} rmr_fc16_lane;

typedef struct rmr_fc16_context {
    rmr_fc16_lane lane[RMR_FC16_LANES];
    rmr_fc16_sha256_ctx sha;
    rmr_fc16_u32 root_crc_state;
    rmr_fc16_u32 entry_crc_state;
    rmr_fc16_u32 path_crc_final;
    rmr_fc16_u32 flags;
    rmr_fc16_u64 entry_declared_size;
    rmr_fc16_u64 entry_seen_size;
    rmr_fc16_u64 entry_count;
    rmr_fc16_u64 total_bytes;
    rmr_fc16_u64 hw_stamp;
    rmr_fc16_u32 thermal_before_milli_c;
    rmr_fc16_u32 thermal_after_milli_c;
    rmr_fc16_u32 thermal_limit_milli_c;
    rmr_fc16_u32 thermal_max_delta_milli_c;
    rmr_fc16_u8 entry_open;
    rmr_fc16_u8 entry_base_lane;
    rmr_fc16_u16 reserved;
} rmr_fc16_context;

typedef struct rmr_fc16_snapshot {
    rmr_fc16_u32 version;
    rmr_fc16_u32 flags;
    rmr_fc16_u32 root_crc32c;
    rmr_fc16_u32 path_count_low;
    rmr_fc16_u64 entry_count;
    rmr_fc16_u64 total_bytes;
    rmr_fc16_u64 hw_stamp;
    rmr_fc16_u32 thermal_before_milli_c;
    rmr_fc16_u32 thermal_after_milli_c;
    rmr_fc16_u32 warmup_rounds;
    rmr_fc16_u32 benchmark_samples;
    rmr_fc16_u64 benchmark_a_ns;
    rmr_fc16_u64 benchmark_b_ns;
    rmr_fc16_u32 lane_crc32c[RMR_FC16_LANES];
    rmr_fc16_u32 lane_xor_fold[RMR_FC16_LANES];
    rmr_fc16_u64 lane_bytes[RMR_FC16_LANES];
    rmr_fc16_u8 sha256[RMR_FC16_SHA256_BYTES];
} rmr_fc16_snapshot;

typedef struct rmr_fc16_delta64 {
    rmr_fc16_u64 word_index;
    rmr_fc16_u64 xor_mask;
} rmr_fc16_delta64;

void rmr_fc16_init(rmr_fc16_context *ctx,
                   rmr_fc16_u64 hw_stamp,
                   rmr_fc16_u32 thermal_before_milli_c,
                   rmr_fc16_u32 thermal_limit_milli_c,
                   rmr_fc16_u32 thermal_max_delta_milli_c);

rmr_fc16_u32 rmr_fc16_entry_begin(rmr_fc16_context *ctx,
                                  const rmr_fc16_u8 *canonical_path,
                                  rmr_fc16_u32 path_len,
                                  rmr_fc16_u64 declared_size,
                                  rmr_fc16_u32 entry_kind);

rmr_fc16_u32 rmr_fc16_entry_data(rmr_fc16_context *ctx,
                                 const rmr_fc16_u8 *data,
                                 rmr_fc16_usize len);

rmr_fc16_u32 rmr_fc16_entry_end(rmr_fc16_context *ctx);

rmr_fc16_u32 rmr_fc16_finalize(rmr_fc16_context *ctx,
                               rmr_fc16_u32 thermal_after_milli_c,
                               rmr_fc16_snapshot *out);

rmr_fc16_u32 rmr_fc16_snapshot_equal(const rmr_fc16_snapshot *a,
                                     const rmr_fc16_snapshot *b);

/* Sparse bit geometry: output only words whose XOR mask is nonzero. */
rmr_fc16_u32 rmr_fc16_delta64_build(const rmr_fc16_u64 *before,
                                    const rmr_fc16_u64 *after,
                                    rmr_fc16_usize word_count,
                                    rmr_fc16_delta64 *out,
                                    rmr_fc16_usize out_capacity,
                                    rmr_fc16_usize *out_count);

rmr_fc16_u32 rmr_fc16_delta64_apply(rmr_fc16_u64 *target,
                                    rmr_fc16_usize word_count,
                                    const rmr_fc16_delta64 *delta,
                                    rmr_fc16_usize delta_count);

/* Fixed-memory warm-up. It changes only caller-provided scratch. */
void rmr_fc16_warmup(rmr_fc16_u8 scratch_a[256],
                     rmr_fc16_u8 scratch_b[256],
                     rmr_fc16_u32 rounds);

/* Canonical binary seal and deterministic ZIP method 0 STORE. */
rmr_fc16_u32 rmr_fc16_seal(const rmr_fc16_snapshot *snapshot,
                           rmr_fc16_u8 *out,
                           rmr_fc16_usize out_capacity,
                           rmr_fc16_usize *out_len);
rmr_fc16_u32 rmr_fc16_verify_seal(const rmr_fc16_u8 *seal,
                                  rmr_fc16_usize seal_len);
rmr_fc16_u32 rmr_fc16_zip_store(const rmr_fc16_u8 *payload,
                                rmr_fc16_usize payload_len,
                                rmr_fc16_u8 *out,
                                rmr_fc16_usize out_capacity,
                                rmr_fc16_usize *out_len);
rmr_fc16_u32 rmr_fc16_zip_verify_roundtrip(const rmr_fc16_u8 *zip,
                                           rmr_fc16_usize zip_len,
                                           const rmr_fc16_u8 *expected,
                                           rmr_fc16_usize expected_len);

/* Deterministic median utility: adapter supplies monotonic samples. */
rmr_fc16_u32 rmr_fc16_median_u64(rmr_fc16_u64 *samples,
                                 rmr_fc16_u32 count,
                                 rmr_fc16_u64 *median_out);

#endif
