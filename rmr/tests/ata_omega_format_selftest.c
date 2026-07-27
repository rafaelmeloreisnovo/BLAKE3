/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "../runtime/ata_omega_format.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static unsigned char g_stream[2048];
static unsigned long g_stream_len;
static unsigned long g_stream_pos;

static long memory_read(int fd, void *out, unsigned long len) {
    (void)fd;
    if (g_stream_pos >= g_stream_len) return 0;
    if (len > g_stream_len - g_stream_pos) len = g_stream_len - g_stream_pos;
    memcpy(out, g_stream + g_stream_pos, len);
    g_stream_pos += len;
    return (long)len;
}

static void build_records(unsigned char *out) {
    unsigned long k;
    for (k = 1; k <= RMR_ATA_RECORD_COUNT; k++) {
        rmr_ata_encode_record(out + (k - 1) * RMR_ATA_RECORD_SIZE,
                              k,
                              0x1000ul + k,
                              ((k - 1) % 9) + 1);
    }
}

static void test_v1(void) {
    unsigned long arena[RMR_ATA_RECORD_COUNT];
    unsigned long sig = 0;
    int format = -1;
    int loaded;

    rmr_ata_build_v1_header(g_stream, 0x1122334455667788ul);
    build_records(g_stream + RMR_ATA_V1_HEADER_SIZE);
    g_stream_len = RMR_ATA_V1_HEADER_SIZE +
                   RMR_ATA_RECORD_COUNT * RMR_ATA_RECORD_SIZE;
    g_stream_pos = 0;

    loaded = rmr_ata_load_fd(0, memory_read, &sig, arena,
                             RMR_ATA_RECORD_COUNT, &format);
    assert(loaded == RMR_ATA_RECORD_COUNT);
    assert(format == RMR_ATA_FORMAT_V1);
    assert(sig == 0x1122334455667788ul);
    assert(arena[0] == 0x1001ul);
    assert(arena[41] == 0x102aul);
}

static void test_legacy(void) {
    unsigned long arena[RMR_ATA_RECORD_COUNT];
    unsigned long sig = 0;
    int format = -1;
    int loaded;

    g_stream[0] = 'R';
    g_stream[1] = 'F';
    g_stream[2] = 'A';
    g_stream[3] = 0;
    rmr_ata_put_u64_le(g_stream + 4, 0x8877665544332211ul);
    build_records(g_stream + RMR_ATA_LEGACY_HEADER_SIZE);
    g_stream_len = RMR_ATA_LEGACY_HEADER_SIZE +
                   RMR_ATA_RECORD_COUNT * RMR_ATA_RECORD_SIZE;
    g_stream_pos = 0;

    loaded = rmr_ata_load_fd(0, memory_read, &sig, arena,
                             RMR_ATA_RECORD_COUNT, &format);
    assert(loaded == RMR_ATA_RECORD_COUNT);
    assert(format == RMR_ATA_FORMAT_LEGACY);
    assert(sig == 0x8877665544332211ul);
    assert(arena[0] == 0x1001ul);
    assert(arena[41] == 0x102aul);
}

int main(void) {
    test_v1();
    test_legacy();
    puts("OK: ATA OMEGA C format selftest");
    return 0;
}
