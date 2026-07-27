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
#define RMR_ATA_FORMAT_LEGACY 0
#define RMR_ATA_FORMAT_V1 1

typedef long (*rmr_ata_read_fn)(int fd, void *buf, unsigned long len);

static inline int rmr_ata_magic_ok(const unsigned char b[4]) {
    return b[0] == 'R' && b[1] == 'F' && b[2] == 'A' && b[3] == 0;
}

static inline void rmr_ata_put_u32_le(unsigned char out[4], unsigned int v) {
    out[0] = (unsigned char)(v & 0xffu);
    out[1] = (unsigned char)((v >> 8) & 0xffu);
    out[2] = (unsigned char)((v >> 16) & 0xffu);
    out[3] = (unsigned char)((v >> 24) & 0xffu);
}

static inline unsigned int rmr_ata_get_u32_le(const unsigned char in[4]) {
    return ((unsigned int)in[0]) |
           ((unsigned int)in[1] << 8) |
           ((unsigned int)in[2] << 16) |
           ((unsigned int)in[3] << 24);
}

static inline void rmr_ata_put_u64_le(unsigned char out[8], unsigned long v) {
    unsigned int i;
    for (i = 0; i < 8u; i++) {
        out[i] = (unsigned char)((v >> (i * 8u)) & 0xfful);
    }
}

static inline unsigned long rmr_ata_get_u64_le(const unsigned char in[8]) {
    unsigned long v = 0;
    unsigned int i;
    for (i = 0; i < 8u; i++) {
        v |= ((unsigned long)in[i]) << (i * 8u);
    }
    return v;
}

static inline void rmr_ata_build_v1_header(unsigned char out[RMR_ATA_V1_HEADER_SIZE],
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

static inline void rmr_ata_encode_record(unsigned char out[RMR_ATA_RECORD_SIZE],
                                         unsigned long k,
                                         unsigned long cyc,
                                         unsigned long par) {
    rmr_ata_put_u64_le(out + 0, k);
    rmr_ata_put_u64_le(out + 8, cyc);
    rmr_ata_put_u64_le(out + 16, par);
}

static inline void rmr_ata_decode_record(const unsigned char in[RMR_ATA_RECORD_SIZE],
                                         unsigned long *k,
                                         unsigned long *cyc,
                                         unsigned long *par) {
    *k = rmr_ata_get_u64_le(in + 0);
    *cyc = rmr_ata_get_u64_le(in + 8);
    *par = rmr_ata_get_u64_le(in + 16);
}

/* Returns 1 on success, 0 on clean EOF before any byte, and -1 on error/truncation. */
static inline int rmr_ata_read_exact(int fd,
                                     rmr_ata_read_fn read_fn,
                                     void *buf,
                                     unsigned long len) {
    unsigned char *out = (unsigned char *)buf;
    unsigned long done = 0;
    while (done < len) {
        long got = read_fn(fd, out + done, len - done);
        if (got < 0) return -1;
        if (got == 0) return done == 0 ? 0 : -1;
        done += (unsigned long)got;
    }
    return 1;
}

static inline int rmr_ata_record_valid(unsigned long k, unsigned long par) {
    return k >= 1ul && k <= RMR_ATA_RECORD_COUNT && par >= 1ul && par <= 9ul;
}

/*
 * Reads either the historical compact stream:
 *   magic[4] + hw_sig[8] + records
 * or the canonical V1 stream:
 *   magic[4] + version[4] + record_size[4] + count[4] + reserved[4]
 *   + hw_sig[8] + records
 */
static inline int rmr_ata_load_fd(int fd,
                                  rmr_ata_read_fn read_fn,
                                  unsigned long *hw_sig_out,
                                  unsigned long *arena,
                                  int cap,
                                  int *format_out) {
    unsigned char magic[4];
    unsigned char probe[24];
    unsigned char record[RMR_ATA_RECORD_SIZE];
    unsigned long k;
    unsigned long cyc;
    unsigned long par;
    int loaded = 0;
    int status;

    if (!read_fn || !hw_sig_out || !arena || cap <= 0) return -1;
    status = rmr_ata_read_exact(fd, read_fn, magic, 4u);
    if (status != 1) return -2;
    if (!rmr_ata_magic_ok(magic)) return -3;

    status = rmr_ata_read_exact(fd, read_fn, probe, sizeof(probe));
    if (status != 1) return -4;

    if (rmr_ata_get_u32_le(probe + 0) == RMR_ATA_V1_VERSION &&
        rmr_ata_get_u32_le(probe + 4) == RMR_ATA_RECORD_SIZE &&
        rmr_ata_get_u32_le(probe + 8) >= 1u &&
        rmr_ata_get_u32_le(probe + 8) <= RMR_ATA_RECORD_COUNT &&
        rmr_ata_get_u32_le(probe + 12) == 0u) {
        unsigned int count = rmr_ata_get_u32_le(probe + 8);
        if ((int)count > cap) return -5;
        *hw_sig_out = rmr_ata_get_u64_le(probe + 16);
        if (format_out) *format_out = RMR_ATA_FORMAT_V1;

        while (loaded < (int)count) {
            status = rmr_ata_read_exact(fd, read_fn, record, RMR_ATA_RECORD_SIZE);
            if (status != 1) return -6;
            rmr_ata_decode_record(record, &k, &cyc, &par);
            if (!rmr_ata_record_valid(k, par) || k != (unsigned long)(loaded + 1)) return -7;
            arena[loaded++] = cyc;
        }
        return loaded;
    }

    /* Legacy: probe = hw_sig[8] + first 16 bytes of record 1. */
    *hw_sig_out = rmr_ata_get_u64_le(probe + 0);
    if (format_out) *format_out = RMR_ATA_FORMAT_LEGACY;
    {
        unsigned char tail[8];
        unsigned int i;
        status = rmr_ata_read_exact(fd, read_fn, tail, sizeof(tail));
        if (status != 1) return -8;
        for (i = 0; i < 16u; i++) record[i] = probe[8u + i];
        for (i = 0; i < 8u; i++) record[16u + i] = tail[i];
    }
    rmr_ata_decode_record(record, &k, &cyc, &par);
    if (!rmr_ata_record_valid(k, par)) return -9;
    arena[loaded++] = cyc;

    while (loaded < cap) {
        status = rmr_ata_read_exact(fd, read_fn, record, RMR_ATA_RECORD_SIZE);
        if (status == 0) break;
        if (status < 0) return -10;
        rmr_ata_decode_record(record, &k, &cyc, &par);
        if (!rmr_ata_record_valid(k, par)) return -11;
        arena[loaded++] = cyc;
    }
    return loaded;
}

#endif
