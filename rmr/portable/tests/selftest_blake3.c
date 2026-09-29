/*
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 */
#include "../include/rmr_portable_blake3_v1.h"

static const rmr_pv1_b3_u8 expect[32] = {
    0x64,0x37,0xb3,0xac,0x38,0x46,0x51,0x33,
    0xff,0xb6,0x3b,0x75,0x27,0x3a,0x8d,0xb5,
    0x48,0xc5,0x58,0x46,0x5d,0x79,0xdb,0x03,
    0xfd,0x35,0x9c,0x6c,0xd5,0xbd,0x9d,0x85
};

int main(void) {
    static const rmr_pv1_b3_u8 abc[3] = { 'a','b','c' };
    rmr_pv1_b3_u8 out[32];
    unsigned int d = 0u;
    unsigned int i;
    if (rmr_pv1_blake3_256(abc, 3u, out) != 0) return 1;
    for (i = 0u; i < 32u; ++i) d |= (unsigned int)(out[i] ^ expect[i]);
    return d == 0u ? 0 : 2;
}
