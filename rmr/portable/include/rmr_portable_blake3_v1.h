/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * Adapter API only. BLAKE3 algorithm/digest semantics remain upstream.
 */
#ifndef RMR_PORTABLE_BLAKE3_V1_H
#define RMR_PORTABLE_BLAKE3_V1_H

#if !defined(__GNUC__) && !defined(__clang__)
#error "RMR Portable BLAKE3 adapter requires GCC/Clang-compatible builtin types"
#endif

typedef __UINT8_TYPE__ rmr_pv1_b3_u8;
typedef __SIZE_TYPE__ rmr_pv1_b3_usize;

#define RMR_PV1_BLAKE3_OUT_BYTES 32u

int rmr_pv1_blake3_256(const rmr_pv1_b3_u8 *input,
                       rmr_pv1_b3_usize input_len,
                       rmr_pv1_b3_u8 out[RMR_PV1_BLAKE3_OUT_BYTES]);

#endif
