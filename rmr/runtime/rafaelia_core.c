/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "../hwif/include/rmr_hwif.h"
#include "ata_omega_format.h"

typedef unsigned long u64;
typedef long s64;

static const rmr_hwif_ops* g_hwif;

#if defined(RMR_ARCH_AARCH64)
extern u64 rmr_hwif_aarch64_midr_raw(void);
extern u64 rmr_hwif_aarch64_mpidr_raw(void);

static s64 sys_openat(int dfd, const char* path, int flags, int mode) {
    register s64 x0 asm("x0") = dfd;
    register const char* x1 asm("x1") = path;
    register s64 x2 asm("x2") = flags;
    register s64 x3 asm("x3") = mode;
    register s64 x8 asm("x8") = 56;
    asm volatile("svc #0" : "+r"(x0) : "r"(x1), "r"(x2), "r"(x3), "r"(x8) : "memory");
    return x0;
}

static s64 sys_close(int fd) {
    register s64 x0 asm("x0") = fd;
    register s64 x8 asm("x8") = 57;
    asm volatile("svc #0" : "+r"(x0) : "r"(x8) : "memory");
    return x0;
}

static s64 sys_fsync(int fd) {
    register s64 x0 asm("x0") = fd;
    register s64 x8 asm("x8") = 82;
    asm volatile("svc #0" : "+r"(x0) : "r"(x8) : "memory");
    return x0;
}
#elif defined(RMR_ARCH_X86_64)
static s64 sys_openat(int dfd, const char* path, int flags, int mode) {
    register s64 rax asm("rax") = 257;
    register s64 rdi asm("rdi") = dfd;
    register const char* rsi asm("rsi") = path;
    register s64 rdx asm("rdx") = flags;
    register s64 r10 asm("r10") = mode;
    asm volatile("syscall"
                 : "+r"(rax)
                 : "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10)
                 : "rcx", "r11", "memory");
    return rax;
}

static s64 sys_close(int fd) {
    register s64 rax asm("rax") = 3;
    register s64 rdi asm("rdi") = fd;
    asm volatile("syscall" : "+r"(rax) : "r"(rdi) : "rcx", "r11", "memory");
    return rax;
}

static s64 sys_fsync(int fd) {
    register s64 rax asm("rax") = 74;
    register s64 rdi asm("rdi") = fd;
    asm volatile("syscall" : "+r"(rax) : "r"(rdi) : "rcx", "r11", "memory");
    return rax;
}
#else
static s64 sys_openat(int dfd, const char* path, int flags, int mode) {
    (void)dfd;
    (void)path;
    (void)flags;
    (void)mode;
    return -1;
}
static s64 sys_close(int fd) { (void)fd; return -1; }
static s64 sys_fsync(int fd) { (void)fd; return -1; }
#endif

static void raw_print(const char* s) {
    u64 n = 0;
    while (s[n]) n++;
    (void)g_hwif->raw_write(1, s, n);
}

static int mem_write_all(int fd, const void* p, u64 n) {
    const unsigned char* cur = (const unsigned char*)p;
    u64 done = 0;
    while (done < n) {
        s64 wrote = g_hwif->raw_write(fd, cur + done, n - done);
        if (wrote <= 0) return -1;
        done += (u64)wrote;
    }
    return 0;
}

static void v20(u64 x, char out[24]) {
    const char g[] = "0123456789ABCDEFGHIJ";
    char t[24];
    int i = 0;
    int j = 0;
    if (x == 0) {
        out[0] = '0';
        out[1] = 0;
        return;
    }
    while (x) {
        t[i++] = g[x % 20];
        x /= 20;
    }
    while (i) out[j++] = t[--i];
    out[j] = 0;
}

static int p9(u64 n) { return n ? (1 + (int)((n - 1) % 9)) : 0; }

void main(void) {
    const int AT_FDCWD = -100;
    const int O_WRONLY = 1;
    const int O_CREAT = 0100;
    const int O_TRUNC = 01000;
    unsigned char header[RMR_ATA_V1_HEADER_SIZE];
    unsigned char record[RMR_ATA_RECORD_SIZE];
    int write_ok;

    g_hwif = rmr_hwif_bootstrap();

    s64 fd = sys_openat(AT_FDCWD, "ATA_OMEGA.bin", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    u64 cpu_id = g_hwif->read_cpu_id_raw();
    u64 hw_sig64 = cpu_id;

    char cpu_v20[24], sig_v20[24];
    v20(cpu_id, cpu_v20);
    v20(hw_sig64, sig_v20);

    raw_print("\n[RAFAELIA_OMEGA] ATA(V1) :: AUTO-ID64 + 42\n");
    raw_print("BACKEND: "); raw_print(g_hwif->backend_name); raw_print("\n");
#if defined(RMR_ARCH_AARCH64)
    {
        u64 midr = rmr_hwif_aarch64_midr_raw();
        u64 mpidr = rmr_hwif_aarch64_mpidr_raw();
        char midr_v20[24], mpidr_v20[24];
        v20(midr, midr_v20);
        v20(mpidr, mpidr_v20);
        raw_print("MIDR(V20): "); raw_print(midr_v20); raw_print("\n");
        raw_print("MPIDR(V20): "); raw_print(mpidr_v20); raw_print("\n");
    }
#endif
    raw_print("CPU_ID_RAW(V20): "); raw_print(cpu_v20); raw_print("\n");
    raw_print("HW_SIG64(V20): "); raw_print(sig_v20); raw_print("\n");
    raw_print("ATA_FORMAT: V1 header=28 record=24 count=42 LE\n");
    raw_print("--------------------------------------------------------------\n");

    write_ok = fd >= 0;
    if (write_ok) {
        rmr_ata_build_v1_header(header, hw_sig64);
        write_ok = mem_write_all((int)fd, header, RMR_ATA_V1_HEADER_SIZE) == 0;
    }

    for (u64 k = 1; k <= RMR_ATA_RECORD_COUNT; k++) {
        u64 cyc = g_hwif->read_time_raw();
        u64 par = (u64)p9(cyc + k);

        if (write_ok) {
            rmr_ata_encode_record(record, k, cyc, par);
            write_ok = mem_write_all((int)fd, record, RMR_ATA_RECORD_SIZE) == 0;
        }

        char cyc_v20[24];
        char id[5];
        char pc[2];
        v20(cyc, cyc_v20);
        id[0] = (char)((k / 100) + '0');
        id[1] = (char)(((k / 10) % 10) + '0');
        id[2] = (char)((k % 10) + '0');
        id[3] = 0;
        pc[0] = (char)('0' + (int)par);
        pc[1] = 0;

        raw_print("[CHAVE:"); raw_print(id); raw_print("] [CYC:");
        raw_print(cyc_v20); raw_print("] [P:"); raw_print(pc); raw_print("] ");
        if (par == 9) raw_print("SINGULARIDADE_OMEGA\n");
        else if (par == 3 || par == 6) raw_print("RESSONANCIA\n");
        else raw_print("FLUXO\n");
    }

    if (fd >= 0) {
        if (write_ok && sys_fsync((int)fd) < 0) write_ok = 0;
        (void)sys_close((int)fd);
    }

    raw_print("--------------------------------------------------------------\n");
    if (write_ok) raw_print("OK: ATA_OMEGA.bin V1 gravada.\n");
    else raw_print("ERRO: ATA_OMEGA.bin incompleta ou nao gravada.\n");
}
