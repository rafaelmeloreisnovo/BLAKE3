/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * RMR adapter only. Upstream BLAKE3 code is not relicensed here.
 */
#include "../../include/rmr_portable_blake3_v1.h"
#include "blake3.h"

int rmr_pv1_blake3_256(const rmr_pv1_b3_u8 *input,
                       rmr_pv1_b3_usize input_len,
                       rmr_pv1_b3_u8 out[32]) {
    blake3_hasher h;
    if (out == (rmr_pv1_b3_u8 *)0) return -1;
    if (input == (const rmr_pv1_b3_u8 *)0 && input_len != 0u) return -1;
    blake3_hasher_init(&h);
    blake3_hasher_update(&h, input, input_len);
    blake3_hasher_finalize(&h, out, 32u);
    return 0;
}
