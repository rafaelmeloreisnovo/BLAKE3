/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 * Fully unrolled SHA-256 compression core.
 */
#include "../include/rmr_cf140_sha256.h"

#define RMR_CF_CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define RMR_CF_MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define RMR_CF_BSIG0(x) (rmr_cf_rotr32((x), 2u) ^ rmr_cf_rotr32((x), 13u) ^ rmr_cf_rotr32((x), 22u))
#define RMR_CF_BSIG1(x) (rmr_cf_rotr32((x), 6u) ^ rmr_cf_rotr32((x), 11u) ^ rmr_cf_rotr32((x), 25u))
#define RMR_CF_SSIG0(x) (rmr_cf_rotr32((x), 7u) ^ rmr_cf_rotr32((x), 18u) ^ ((x) >> 3u))
#define RMR_CF_SSIG1(x) (rmr_cf_rotr32((x), 17u) ^ rmr_cf_rotr32((x), 19u) ^ ((x) >> 10u))
#define RMR_CF_ROUND(a,b,c,d,e,f,g,h,k,wv) do { \
    const rmr_cf_u32 t1 = (h) + RMR_CF_BSIG1(e) + RMR_CF_CH((e),(f),(g)) + (k) + (wv); \
    const rmr_cf_u32 t2 = RMR_CF_BSIG0(a) + RMR_CF_MAJ((a),(b),(c)); \
    (d) += t1; \
    (h) = t1 + t2; \
} while (0)

void rmr_cf_sha256_init(rmr_cf_sha256_state *state) {
    state->h0 = 0x6a09e667u; state->h1 = 0xbb67ae85u;
    state->h2 = 0x3c6ef372u; state->h3 = 0xa54ff53au;
    state->h4 = 0x510e527fu; state->h5 = 0x9b05688cu;
    state->h6 = 0x1f83d9abu; state->h7 = 0x5be0cd19u;
}

void rmr_cf_sha256_compress_block(rmr_cf_sha256_state *state, const rmr_cf_u8 block[64]) {
    rmr_cf_u32 w[64];
    rmr_cf_u32 a = state->h0; rmr_cf_u32 b = state->h1;
    rmr_cf_u32 c = state->h2; rmr_cf_u32 d = state->h3;
    rmr_cf_u32 e = state->h4; rmr_cf_u32 f = state->h5;
    rmr_cf_u32 g = state->h6; rmr_cf_u32 h = state->h7;

    w[0] = rmr_cf_load32_be(block + 0);
    w[1] = rmr_cf_load32_be(block + 4);
    w[2] = rmr_cf_load32_be(block + 8);
    w[3] = rmr_cf_load32_be(block + 12);
    w[4] = rmr_cf_load32_be(block + 16);
    w[5] = rmr_cf_load32_be(block + 20);
    w[6] = rmr_cf_load32_be(block + 24);
    w[7] = rmr_cf_load32_be(block + 28);
    w[8] = rmr_cf_load32_be(block + 32);
    w[9] = rmr_cf_load32_be(block + 36);
    w[10] = rmr_cf_load32_be(block + 40);
    w[11] = rmr_cf_load32_be(block + 44);
    w[12] = rmr_cf_load32_be(block + 48);
    w[13] = rmr_cf_load32_be(block + 52);
    w[14] = rmr_cf_load32_be(block + 56);
    w[15] = rmr_cf_load32_be(block + 60);
    w[16] = RMR_CF_SSIG1(w[14]) + w[9] + RMR_CF_SSIG0(w[1]) + w[0];
    w[17] = RMR_CF_SSIG1(w[15]) + w[10] + RMR_CF_SSIG0(w[2]) + w[1];
    w[18] = RMR_CF_SSIG1(w[16]) + w[11] + RMR_CF_SSIG0(w[3]) + w[2];
    w[19] = RMR_CF_SSIG1(w[17]) + w[12] + RMR_CF_SSIG0(w[4]) + w[3];
    w[20] = RMR_CF_SSIG1(w[18]) + w[13] + RMR_CF_SSIG0(w[5]) + w[4];
    w[21] = RMR_CF_SSIG1(w[19]) + w[14] + RMR_CF_SSIG0(w[6]) + w[5];
    w[22] = RMR_CF_SSIG1(w[20]) + w[15] + RMR_CF_SSIG0(w[7]) + w[6];
    w[23] = RMR_CF_SSIG1(w[21]) + w[16] + RMR_CF_SSIG0(w[8]) + w[7];
    w[24] = RMR_CF_SSIG1(w[22]) + w[17] + RMR_CF_SSIG0(w[9]) + w[8];
    w[25] = RMR_CF_SSIG1(w[23]) + w[18] + RMR_CF_SSIG0(w[10]) + w[9];
    w[26] = RMR_CF_SSIG1(w[24]) + w[19] + RMR_CF_SSIG0(w[11]) + w[10];
    w[27] = RMR_CF_SSIG1(w[25]) + w[20] + RMR_CF_SSIG0(w[12]) + w[11];
    w[28] = RMR_CF_SSIG1(w[26]) + w[21] + RMR_CF_SSIG0(w[13]) + w[12];
    w[29] = RMR_CF_SSIG1(w[27]) + w[22] + RMR_CF_SSIG0(w[14]) + w[13];
    w[30] = RMR_CF_SSIG1(w[28]) + w[23] + RMR_CF_SSIG0(w[15]) + w[14];
    w[31] = RMR_CF_SSIG1(w[29]) + w[24] + RMR_CF_SSIG0(w[16]) + w[15];
    w[32] = RMR_CF_SSIG1(w[30]) + w[25] + RMR_CF_SSIG0(w[17]) + w[16];
    w[33] = RMR_CF_SSIG1(w[31]) + w[26] + RMR_CF_SSIG0(w[18]) + w[17];
    w[34] = RMR_CF_SSIG1(w[32]) + w[27] + RMR_CF_SSIG0(w[19]) + w[18];
    w[35] = RMR_CF_SSIG1(w[33]) + w[28] + RMR_CF_SSIG0(w[20]) + w[19];
    w[36] = RMR_CF_SSIG1(w[34]) + w[29] + RMR_CF_SSIG0(w[21]) + w[20];
    w[37] = RMR_CF_SSIG1(w[35]) + w[30] + RMR_CF_SSIG0(w[22]) + w[21];
    w[38] = RMR_CF_SSIG1(w[36]) + w[31] + RMR_CF_SSIG0(w[23]) + w[22];
    w[39] = RMR_CF_SSIG1(w[37]) + w[32] + RMR_CF_SSIG0(w[24]) + w[23];
    w[40] = RMR_CF_SSIG1(w[38]) + w[33] + RMR_CF_SSIG0(w[25]) + w[24];
    w[41] = RMR_CF_SSIG1(w[39]) + w[34] + RMR_CF_SSIG0(w[26]) + w[25];
    w[42] = RMR_CF_SSIG1(w[40]) + w[35] + RMR_CF_SSIG0(w[27]) + w[26];
    w[43] = RMR_CF_SSIG1(w[41]) + w[36] + RMR_CF_SSIG0(w[28]) + w[27];
    w[44] = RMR_CF_SSIG1(w[42]) + w[37] + RMR_CF_SSIG0(w[29]) + w[28];
    w[45] = RMR_CF_SSIG1(w[43]) + w[38] + RMR_CF_SSIG0(w[30]) + w[29];
    w[46] = RMR_CF_SSIG1(w[44]) + w[39] + RMR_CF_SSIG0(w[31]) + w[30];
    w[47] = RMR_CF_SSIG1(w[45]) + w[40] + RMR_CF_SSIG0(w[32]) + w[31];
    w[48] = RMR_CF_SSIG1(w[46]) + w[41] + RMR_CF_SSIG0(w[33]) + w[32];
    w[49] = RMR_CF_SSIG1(w[47]) + w[42] + RMR_CF_SSIG0(w[34]) + w[33];
    w[50] = RMR_CF_SSIG1(w[48]) + w[43] + RMR_CF_SSIG0(w[35]) + w[34];
    w[51] = RMR_CF_SSIG1(w[49]) + w[44] + RMR_CF_SSIG0(w[36]) + w[35];
    w[52] = RMR_CF_SSIG1(w[50]) + w[45] + RMR_CF_SSIG0(w[37]) + w[36];
    w[53] = RMR_CF_SSIG1(w[51]) + w[46] + RMR_CF_SSIG0(w[38]) + w[37];
    w[54] = RMR_CF_SSIG1(w[52]) + w[47] + RMR_CF_SSIG0(w[39]) + w[38];
    w[55] = RMR_CF_SSIG1(w[53]) + w[48] + RMR_CF_SSIG0(w[40]) + w[39];
    w[56] = RMR_CF_SSIG1(w[54]) + w[49] + RMR_CF_SSIG0(w[41]) + w[40];
    w[57] = RMR_CF_SSIG1(w[55]) + w[50] + RMR_CF_SSIG0(w[42]) + w[41];
    w[58] = RMR_CF_SSIG1(w[56]) + w[51] + RMR_CF_SSIG0(w[43]) + w[42];
    w[59] = RMR_CF_SSIG1(w[57]) + w[52] + RMR_CF_SSIG0(w[44]) + w[43];
    w[60] = RMR_CF_SSIG1(w[58]) + w[53] + RMR_CF_SSIG0(w[45]) + w[44];
    w[61] = RMR_CF_SSIG1(w[59]) + w[54] + RMR_CF_SSIG0(w[46]) + w[45];
    w[62] = RMR_CF_SSIG1(w[60]) + w[55] + RMR_CF_SSIG0(w[47]) + w[46];
    w[63] = RMR_CF_SSIG1(w[61]) + w[56] + RMR_CF_SSIG0(w[48]) + w[47];
    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0x428a2f98u, w[0]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0x71374491u, w[1]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0xb5c0fbcfu, w[2]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0xe9b5dba5u, w[3]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x3956c25bu, w[4]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0x59f111f1u, w[5]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x923f82a4u, w[6]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0xab1c5ed5u, w[7]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0xd807aa98u, w[8]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0x12835b01u, w[9]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0x243185beu, w[10]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0x550c7dc3u, w[11]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x72be5d74u, w[12]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0x80deb1feu, w[13]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x9bdc06a7u, w[14]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0xc19bf174u, w[15]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0xe49b69c1u, w[16]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0xefbe4786u, w[17]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0x0fc19dc6u, w[18]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0x240ca1ccu, w[19]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x2de92c6fu, w[20]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0x4a7484aau, w[21]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x5cb0a9dcu, w[22]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0x76f988dau, w[23]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0x983e5152u, w[24]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0xa831c66du, w[25]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0xb00327c8u, w[26]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0xbf597fc7u, w[27]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0xc6e00bf3u, w[28]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0xd5a79147u, w[29]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x06ca6351u, w[30]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0x14292967u, w[31]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0x27b70a85u, w[32]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0x2e1b2138u, w[33]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0x4d2c6dfcu, w[34]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0x53380d13u, w[35]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x650a7354u, w[36]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0x766a0abbu, w[37]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x81c2c92eu, w[38]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0x92722c85u, w[39]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0xa2bfe8a1u, w[40]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0xa81a664bu, w[41]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0xc24b8b70u, w[42]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0xc76c51a3u, w[43]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0xd192e819u, w[44]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0xd6990624u, w[45]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0xf40e3585u, w[46]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0x106aa070u, w[47]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0x19a4c116u, w[48]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0x1e376c08u, w[49]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0x2748774cu, w[50]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0x34b0bcb5u, w[51]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x391c0cb3u, w[52]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0x4ed8aa4au, w[53]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0x5b9cca4fu, w[54]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0x682e6ff3u, w[55]);\n    RMR_CF_ROUND(a, b, c, d, e, f, g, h, 0x748f82eeu, w[56]);\n    RMR_CF_ROUND(h, a, b, c, d, e, f, g, 0x78a5636fu, w[57]);\n    RMR_CF_ROUND(g, h, a, b, c, d, e, f, 0x84c87814u, w[58]);\n    RMR_CF_ROUND(f, g, h, a, b, c, d, e, 0x8cc70208u, w[59]);\n    RMR_CF_ROUND(e, f, g, h, a, b, c, d, 0x90befffau, w[60]);\n    RMR_CF_ROUND(d, e, f, g, h, a, b, c, 0xa4506cebu, w[61]);\n    RMR_CF_ROUND(c, d, e, f, g, h, a, b, 0xbef9a3f7u, w[62]);\n    RMR_CF_ROUND(b, c, d, e, f, g, h, a, 0xc67178f2u, w[63]);\n

    state->h0 += a; state->h1 += b; state->h2 += c; state->h3 += d;
    state->h4 += e; state->h5 += f; state->h6 += g; state->h7 += h;
}

#undef RMR_CF_ROUND
#undef RMR_CF_SSIG1
#undef RMR_CF_SSIG0
#undef RMR_CF_BSIG1
#undef RMR_CF_BSIG0
#undef RMR_CF_MAJ
#undef RMR_CF_CH
