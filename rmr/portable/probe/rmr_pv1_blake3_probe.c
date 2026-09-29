/*
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
#include "../include/rmr_portable_blake3_v1.h"

static const rmr_pv1_b3_u8 abc[3] = { 'a','b','c' };
static rmr_pv1_b3_u8 out32[32];

void rmr_pv1_blake3_probe_entry(void) {
    (void)rmr_pv1_blake3_256(abc, 3u, out32);
    __builtin_trap();
}
