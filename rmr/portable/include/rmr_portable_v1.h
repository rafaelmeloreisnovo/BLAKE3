/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
#ifndef RMR_PORTABLE_V1_H
#define RMR_PORTABLE_V1_H

#if !defined(__GNUC__) && !defined(__clang__)
#error "RMR Portable V1 requires GCC/Clang-compatible builtin integer types"
#endif

typedef __UINT8_TYPE__  rmr_pv1_u8;
typedef __UINT32_TYPE__ rmr_pv1_u32;

#define RMR_PV1_VERSION 1u
#define RMR_PV1_FRAME_BYTES 256u
#define RMR_PV1_REDUCED_BYTES 16u
#define RMR_PV1_BATCH4_INPUT_BYTES 1024u
#define RMR_PV1_BATCH4_OUTPUT_BYTES 64u

void rmr_pv1_reduce256(rmr_pv1_u8 out[RMR_PV1_REDUCED_BYTES],
                       const rmr_pv1_u8 in[RMR_PV1_FRAME_BYTES]);

void rmr_pv1_batch4(rmr_pv1_u8 out[RMR_PV1_BATCH4_OUTPUT_BYTES],
                    const rmr_pv1_u8 in[RMR_PV1_BATCH4_INPUT_BYTES]);

rmr_pv1_u32 rmr_pv1_equal16(
    const rmr_pv1_u8 a[RMR_PV1_REDUCED_BYTES],
    const rmr_pv1_u8 b[RMR_PV1_REDUCED_BYTES]);

rmr_pv1_u32 rmr_pv1_selftest(void);

#endif
