/*
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
#include "../include/rmr_portable_v1.h"

static rmr_pv1_u8 in256[256];
static rmr_pv1_u8 out16[16];

void rmr_pv1_probe_entry(void) {
    rmr_pv1_reduce256(out16, in256);
    __builtin_trap();
}
