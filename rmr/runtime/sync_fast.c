/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "ata_omega_format.h"

typedef unsigned long u64;
typedef long s64;

extern void sys_write(int fd, const char* buf, u64 len);
extern int  sys_openat(int dfd, const char* path, int flags, int mode);
extern s64  sys_read(int fd, void* buf, u64 len);
extern int  sys_close(int fd);
extern u64 rd_cntfrq(void);
extern u64 rd_cntvct(void);
extern u64 rd_midr(void);

static const char G[] = "0123456789ABCDEFGHIJ";

#define OUT_CAP (128u * 1024u)
static char OUT[OUT_CAP];
static u64 OUTN = 0;

static void b_putc(char c) {
    if (OUTN < OUT_CAP) OUT[OUTN++] = c;
}

static void b_puts(const char* s) {
    u64 i;
    for (i = 0; s[i]; i++) b_putc(s[i]);
}

static void b_putnl(void) { b_putc('\n'); }

static void b_put_u3(int n) {
    b_putc((char)('0' + (n / 100) % 10));
    b_putc((char)('0' + (n / 10) % 10));
    b_putc((char)('0' + n % 10));
}

static void b_put_v20(u64 n) {
    char tmp[64];
    int i = 0;
    if (n == 0) { b_putc('0'); return; }
    while (n) { tmp[i++] = G[n % 20]; n /= 20; }
    while (i) b_putc(tmp[--i]);
}

static int p9(u64 n) {
    if (n == 0) return 0;
    return 1 + (int)((n - 1) % 9);
}

static u64 mix64(u64 x) {
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * 2685821657736338717ULL;
}

static int load_ata(const char* path,
                    u64* hw_sig_out,
                    u64* arena,
                    int cap,
                    int* format_out) {
    int fd = sys_openat(-100, path, 0, 0);
    int loaded;
    if (fd < 0) return -1;
    loaded = rmr_ata_load_fd(fd, sys_read, hw_sig_out, arena, cap, format_out);
    (void)sys_close(fd);
    return loaded;
}

static void hdr(void) {
    u64 cntfrq = rd_cntfrq();
    u64 cntvct = rd_cntvct();
    u64 midr = rd_midr();
    b_puts("[RAFAELIA_SYNC_OMEGA_FAST] ARM64 no-libc :: ATA + CLOCK + 3-6-9"); b_putnl();
    b_puts("CNTFRQ(V20): "); b_put_v20(cntfrq); b_putnl();
    b_puts("CNTVCT(V20): "); b_put_v20(cntvct); b_putnl();
    b_puts("MIDR (V20): "); b_put_v20(midr & 0xffffffffULL); b_putnl();
    b_puts("--------------------------------------------------------------"); b_putnl();
}

void main(void) {
    u64 arena[RMR_ATA_RECORD_COUNT];
    u64 ata_hw = 0;
    int ata_format = -1;
    int loaded;
    u64 midr;
    u64 cntfrq;
    u64 seed;
    int i;

    hdr();
    loaded = load_ata("ATA_OMEGA.bin", &ata_hw, arena,
                      RMR_ATA_RECORD_COUNT, &ata_format);

    if (loaded > 0) {
        b_puts("ATA_FORMAT: ");
        b_puts(ata_format == RMR_ATA_FORMAT_V1 ? "V1_EXTENDED" : "LEGACY_COMPACT");
        b_putnl();
        b_puts("ATA_HW_SIG(V20): "); b_put_v20(ata_hw); b_putnl();
        b_puts("ATA_LOADED: "); b_put_v20((u64)loaded); b_putnl();
        b_puts("CYC0: "); b_put_v20(arena[0]); b_putnl();
        b_puts("CYC1: "); b_put_v20(arena[loaded > 1 ? 1 : 0]); b_putnl();
        b_puts("CYC2: "); b_put_v20(arena[loaded > 2 ? 2 : 0]); b_putnl();
        b_puts("--------------------------------------------------------------"); b_putnl();
    } else {
        b_puts("WARN: ATA_OMEGA.bin invalida/ausente. (clock-only)"); b_putnl();
        b_puts("--------------------------------------------------------------"); b_putnl();
    }

    midr = rd_midr();
    cntfrq = rd_cntfrq();
    seed = 0x524641ULL ^ (midr << 1) ^ (cntfrq << 7);

    for (i = 1; i <= 42; i++) {
        u64 t = rd_cntvct();
        u64 cyc = loaded > 0 ? arena[(i - 1) % loaded] : 0;
        u64 x = mix64(seed ^ t ^ (cyc * 0x9e3779b97f4a7c15ULL) ^ (u64)i);
        int p = p9(x);

        b_puts("[CHAVE:"); b_put_u3(i); b_puts("] [X:");
        b_put_v20(x);
        b_puts("] [P:"); b_putc((char)('0' + p)); b_puts("] ");
        if (p == 9) b_puts("SINGULARIDADE_OMEGA");
        else if (p == 3 || p == 6) b_puts("RESSONANCIA");
        else b_puts("FLUXO");
        b_putnl();
    }

    b_puts("--------------------------------------------------------------"); b_putnl();
    b_puts("OK: SYNC FAST concluido."); b_putnl();
    sys_write(1, OUT, OUTN);
}
