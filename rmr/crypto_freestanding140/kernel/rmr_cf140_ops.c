/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * CF140 fixed-shape substrate.
 * Source-shape invariant: no if/for/while statements in this translation unit.
 */
#include "../include/rmr_cf140.h"

rmr_cf_u32 rmr_cf_load32_le(const rmr_cf_u8 src[4]) {
    return ((rmr_cf_u32)src[0]) |
           ((rmr_cf_u32)src[1] << 8u) |
           ((rmr_cf_u32)src[2] << 16u) |
           ((rmr_cf_u32)src[3] << 24u);
}

rmr_cf_u64 rmr_cf_load64_le(const rmr_cf_u8 src[8]) {
    return ((rmr_cf_u64)src[0]) |
           ((rmr_cf_u64)src[1] << 8u) |
           ((rmr_cf_u64)src[2] << 16u) |
           ((rmr_cf_u64)src[3] << 24u) |
           ((rmr_cf_u64)src[4] << 32u) |
           ((rmr_cf_u64)src[5] << 40u) |
           ((rmr_cf_u64)src[6] << 48u) |
           ((rmr_cf_u64)src[7] << 56u);
}

void rmr_cf_store32_le(rmr_cf_u8 dst[4], rmr_cf_u32 value) {
    dst[0] = (rmr_cf_u8)value;
    dst[1] = (rmr_cf_u8)(value >> 8u);
    dst[2] = (rmr_cf_u8)(value >> 16u);
    dst[3] = (rmr_cf_u8)(value >> 24u);
}

void rmr_cf_store64_le(rmr_cf_u8 dst[8], rmr_cf_u64 value) {
    dst[0] = (rmr_cf_u8)value;
    dst[1] = (rmr_cf_u8)(value >> 8u);
    dst[2] = (rmr_cf_u8)(value >> 16u);
    dst[3] = (rmr_cf_u8)(value >> 24u);
    dst[4] = (rmr_cf_u8)(value >> 32u);
    dst[5] = (rmr_cf_u8)(value >> 40u);
    dst[6] = (rmr_cf_u8)(value >> 48u);
    dst[7] = (rmr_cf_u8)(value >> 56u);
}

void rmr_cf_xor16(rmr_cf_u8 dst[16],
                  const rmr_cf_u8 left[16],
                  const rmr_cf_u8 right[16]) {
    dst[0]  = left[0]  ^ right[0];
    dst[1]  = left[1]  ^ right[1];
    dst[2]  = left[2]  ^ right[2];
    dst[3]  = left[3]  ^ right[3];
    dst[4]  = left[4]  ^ right[4];
    dst[5]  = left[5]  ^ right[5];
    dst[6]  = left[6]  ^ right[6];
    dst[7]  = left[7]  ^ right[7];
    dst[8]  = left[8]  ^ right[8];
    dst[9]  = left[9]  ^ right[9];
    dst[10] = left[10] ^ right[10];
    dst[11] = left[11] ^ right[11];
    dst[12] = left[12] ^ right[12];
    dst[13] = left[13] ^ right[13];
    dst[14] = left[14] ^ right[14];
    dst[15] = left[15] ^ right[15];
}

void rmr_cf_xor32_bytes(rmr_cf_u8 dst[32],
                        const rmr_cf_u8 left[32],
                        const rmr_cf_u8 right[32]) {
    rmr_cf_xor16(dst, left, right);
    rmr_cf_xor16(dst + 16, left + 16, right + 16);
}

void rmr_cf_xor64_bytes(rmr_cf_u8 dst[64],
                        const rmr_cf_u8 left[64],
                        const rmr_cf_u8 right[64]) {
    rmr_cf_xor32_bytes(dst, left, right);
    rmr_cf_xor32_bytes(dst + 32, left + 32, right + 32);
}

rmr_cf_u32 rmr_cf_diff16(const rmr_cf_u8 left[16],
                         const rmr_cf_u8 right[16]) {
    return
        (rmr_cf_u32)(left[0]  ^ right[0])  |
        (rmr_cf_u32)(left[1]  ^ right[1])  |
        (rmr_cf_u32)(left[2]  ^ right[2])  |
        (rmr_cf_u32)(left[3]  ^ right[3])  |
        (rmr_cf_u32)(left[4]  ^ right[4])  |
        (rmr_cf_u32)(left[5]  ^ right[5])  |
        (rmr_cf_u32)(left[6]  ^ right[6])  |
        (rmr_cf_u32)(left[7]  ^ right[7])  |
        (rmr_cf_u32)(left[8]  ^ right[8])  |
        (rmr_cf_u32)(left[9]  ^ right[9])  |
        (rmr_cf_u32)(left[10] ^ right[10]) |
        (rmr_cf_u32)(left[11] ^ right[11]) |
        (rmr_cf_u32)(left[12] ^ right[12]) |
        (rmr_cf_u32)(left[13] ^ right[13]) |
        (rmr_cf_u32)(left[14] ^ right[14]) |
        (rmr_cf_u32)(left[15] ^ right[15]);
}

rmr_cf_u32 rmr_cf_diff32(const rmr_cf_u8 left[32],
                         const rmr_cf_u8 right[32]) {
    return rmr_cf_diff16(left, right) |
           rmr_cf_diff16(left + 16, right + 16);
}
