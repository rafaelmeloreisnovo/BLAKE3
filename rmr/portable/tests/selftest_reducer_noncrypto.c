/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * Source-side negative control: this fixed-size XOR reducer is not a
 * cryptographic hash, MAC, KDF, signature, or integrity authenticator.
 * PASS means an expected constructed collision was observed.
 */
#include "../include/rmr_portable_v1.h"

int main(void) {
    rmr_pv1_u8 all_zero[RMR_PV1_FRAME_BYTES] = {0};
    rmr_pv1_u8 cancelling_pair[RMR_PV1_FRAME_BYTES] = {0};
    rmr_pv1_u8 left[RMR_PV1_FRAME_BYTES] = {0};
    rmr_pv1_u8 right[RMR_PV1_FRAME_BYTES] = {0};
    rmr_pv1_u8 a[RMR_PV1_REDUCED_BYTES];
    rmr_pv1_u8 b[RMR_PV1_REDUCED_BYTES];

    /* Two byte changes in the same XOR column cancel completely. */
    cancelling_pair[0] = 1u;
    cancelling_pair[16] = 1u;
    if (cancelling_pair[0] == all_zero[0] ||
        cancelling_pair[16] == all_zero[16]) return 1;
    rmr_pv1_reduce256(a, all_zero);
    rmr_pv1_reduce256(b, cancelling_pair);
    if (rmr_pv1_equal16(a, b) != 1u) return 2;

    /* Two distinct single-byte inputs produce the same nonzero result. */
    left[0] = 0x80u;
    right[16] = 0x80u;
    if (left[0] == right[0] || left[16] == right[16]) return 3;
    rmr_pv1_reduce256(a, left);
    rmr_pv1_reduce256(b, right);
    if (rmr_pv1_equal16(a, b) != 1u) return 4;
    if (a[0] != 0x80u) return 5;

    return 0;
}
