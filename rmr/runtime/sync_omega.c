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

static void print(const char* s) {
    u64 n = 0;
    while (s[n]) n++;
    sys_write(1, s, n);
}

static void print_nl(void) { sys_write(1, "\n", 1); }

static void v20(u64 n, char* out) {
    static const char g[] = "0123456789ABCDEFGHIJ";
    char tmp[64];
    int i = 0;
    int j = 0;
    if (n == 0) { out[0] = '0'; out[1] = 0; return; }
    while (n) { tmp[i++] = g[n % 20]; n /= 20; }
    while (i) out[j++] = tmp[--i];
    out[j] = 0;
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

static void print_hdr(const char* title) {
    print("\n"); print(title); print("\n");
}

void main(void) {
    u64 cntfrq = rd_cntfrq();
    u64 cntvct0 = rd_cntvct();
    u64 midr = rd_midr();
    char b_cntfrq[64], b_cntvct[64], b_midr[64];
    u64 arena[RMR_ATA_RECORD_COUNT];
    u64 ata_hw_sig = 0;
    int ata_format = -1;
    int loaded;

    v20(cntfrq, b_cntfrq);
    v20(cntvct0, b_cntvct);
    v20(midr & 0xffffffffULL, b_midr);

    print_hdr("[RAFAELIA_SYNC_OMEGA] ARM64 no-libc :: ATA + CLOCK + 3-6-9");
    print("CNTFRQ(V20): "); print(b_cntfrq); print_nl();
    print("CNTVCT(V20): "); print(b_cntvct); print_nl();
    print("MIDR (V20): "); print(b_midr); print_nl();
    print("--------------------------------------------------------------\n");

    loaded = load_ata("ATA_OMEGA.bin", &ata_hw_sig, arena,
                      RMR_ATA_RECORD_COUNT, &ata_format);

    if (loaded > 0) {
        char hw20[64];
        char ld[8];
        char c0[64], c1[64], c2[64];
        v20(ata_hw_sig, hw20);
        print("ATA_FORMAT: ");
        print(ata_format == RMR_ATA_FORMAT_V1 ? "V1_EXTENDED" : "LEGACY_COMPACT");
        print_nl();
        print("ATA_HW_SIG(V20): "); print(hw20); print_nl();
        print("ATA_LOADED: ");
        ld[0] = (char)('0' + (loaded / 10));
        ld[1] = (char)('0' + (loaded % 10));
        ld[2] = 0;
        print(ld); print_nl();

        v20(arena[0], c0);
        v20(arena[loaded > 1 ? 1 : 0], c1);
        v20(arena[loaded > 2 ? 2 : 0], c2);
        print("CYC0: "); print(c0); print_nl();
        print("CYC1: "); print(c1); print_nl();
        print("CYC2: "); print(c2); print_nl();
        print("--------------------------------------------------------------\n");
    } else {
        print("WARN: ATA_OMEGA.bin invalida/ausente. SYNC em modo clock-only.\n");
        print("--------------------------------------------------------------\n");
    }

    {
        u64 seed = 0x524641ULL ^ (midr << 1) ^ (cntfrq << 7);
        int i;
        for (i = 1; i <= 42; i++) {
            u64 t = rd_cntvct();
            u64 cyc = loaded > 0 ? arena[(i - 1) % loaded] : 0;
            u64 x = mix64(seed ^ t ^ (cyc * 0x9e3779b97f4a7c15ULL) ^ (u64)i);
            int p = p9(x);
            char bx[64];
            char id[8];
            char pc[4];

            v20(x, bx);
            id[0] = (char)('0' + (i / 100) % 10);
            id[1] = (char)('0' + (i / 10) % 10);
            id[2] = (char)('0' + (i % 10));
            id[3] = 0;
            pc[0] = (char)('0' + p);
            pc[1] = 0;

            print("[CHAVE:"); print(id); print("] [X:"); print(bx);
            print("] [P:"); print(pc); print("] ");
            if (p == 9) print("SINGULARIDADE_OMEGA\n");
            else if (p == 3 || p == 6) print("RESSONANCIA\n");
            else print("FLUXO\n");

            for (volatile u64 d = 0; d < 350000; d++);
        }
    }

    print("--------------------------------------------------------------\n");
    print("OK: SYNC concluido.\n");
}
