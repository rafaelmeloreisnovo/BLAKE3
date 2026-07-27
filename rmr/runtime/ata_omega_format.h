/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#ifndef RMR_ATA_OMEGA_FORMAT_H
#define RMR_ATA_OMEGA_FORMAT_H

#define RMR_ATA_MAGIC_SIZE 4u
#define RMR_ATA_V1_VERSION 1u
#define RMR_ATA_RECORD_SIZE 24u
#define RMR_ATA_RECORD_COUNT 42u
#define RMR_ATA_LEGACY_HEADER_SIZE 12u
#define RMR_ATA_V1_HEADER_SIZE 28u

static int rmr_ata_magic_ok(const unsigned char b[4]) {
    return b[0] == 'R' && b[1] == 'F' && b[2] == 'A' && b[3] == 0;
}

static void rmr_ata_put_u32_le(unsigned char out[4], unsigned int v) {
    out[0] = (unsigned char)(v & 0xffu);
    out[1] = (unsigned char)((v >> 8) & 0xffu);
    out[2] = (unsigned char)((v >> 16) & 0xffu);
    out[3] = (unsigned char)((v >> 24) & 0xffu);
}

static unsigned int rmr_ata_get_u32_le(const unsigned char in[4]) {
    return ((unsigned int)in[0]) |
           ((unsigned int)in[1] << 8) |
           ((unsigned int)in[2] << 16) |
           ((unsigned int)in[3] << 24);
}

static void rmr_ata_put_u64_le(unsigned char out[8], unsigned long v) {
    unsigned int i;
    for (i = 0; i < 8u; i++) {
        out[i] = (unsigned char)((v >> (i * 8u)) & 0xfful);
    }
}

static unsigned long rmr_ata_get_u64_le(const unsigned char in[8]) {
    unsigned long v = 0;
    unsigned int i;
    for (i = 0; i < 8u; i++) {
        v |= ((unsigned long)in[i]) << (i * 8u);
    }
    return v;
}

static void rmr_ata_build_v1_header(unsigned char out[RMR_ATA_V1_HEADER_SIZE],
                                    unsigned long hw_sig) {
    out[0] = 'R';
    out[1] = 'F';
    out[2] = 'A';
    out[3] = 0;
    rmr_ata_put_u32_le(out + 4, RMR_ATA_V1_VERSION);
    rmr_ata_put_u32_le(out + 8, RMR_ATA_RECORD_SIZE);
    rmr_ata_put_u32_le(out + 12, RMR_ATA_RECORD_COUNT);
    rmr_ata_put_u32_le(out + 16, 0u);
    rmr_ata_put_u64_le(out + 20, hw_sig);
}

static void rmr_ata_encode_record(unsigned char out[RMR_ATA_RECORD_SIZE],
                                  unsigned long k,
                                  unsigned long cyc,
                                  unsigned long par) {
    rmr_ata_put_u64_le(out + 0, k);
    rmr_ata_put_u64_le(out + 8, cyc);
    rmr_ata_put_u64_le(out + 16, par);
}

static void rmr_ata_decode_record(const unsigned char in[RMR_ATA_RECORD_SIZE],
                                  unsigned long *k,
                                  unsigned long *cyc,
                                  unsigned long *par) {
    *k = rmr_ata_get_u64_le(in + 0);
    *cyc = rmr_ata_get_u64_le(in + 8);
    *par = rmr_ata_get_u64_le(in + 16);
}

#endif
