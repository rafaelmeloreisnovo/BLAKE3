/*
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
#include "../include/rmr_portable_v1.h"

int main(void) {
    rmr_pv1_u8 in[1024] = {0};
    rmr_pv1_u8 out[64] = {0};
    rmr_pv1_u8 one[16] = {0};

    in[0] = 1u;
    in[256u + 17u] = 2u;
    in[512u + 34u] = 4u;
    in[768u + 51u] = 8u;

    rmr_pv1_batch4(out, in);

    one[0] = 1u;
    if (!rmr_pv1_equal16(out + 0u, one)) return 1;
    one[0] = 0u; one[1] = 2u;
    if (!rmr_pv1_equal16(out + 16u, one)) return 2;
    one[1] = 0u; one[2] = 4u;
    if (!rmr_pv1_equal16(out + 32u, one)) return 3;
    one[2] = 0u; one[3] = 8u;
    if (!rmr_pv1_equal16(out + 48u, one)) return 4;
    if (!rmr_pv1_selftest()) return 5;
    return 0;
}
