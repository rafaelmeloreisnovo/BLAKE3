#include "include/rmr_hwif.h"

typedef unsigned long u64;
typedef unsigned int u32;
typedef long s64;

static const rmr_hwif_ops* g_hwif;

#if defined(RMR_ARCH_AARCH64)
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

/* ----------- IO básico ----------- */
static void raw_print(const char* s) {
    u64 n = 0;
    while (s[n]) {
        n++;
    }
    g_hwif->raw_write(1, s, n);
}

static void mem_write(int fd, const void* p, u64 n) {
    g_hwif->raw_write(fd, p, n);
}

/* ----------- Base20 (0-9A-J) ----------- */
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
    while (i) {
        out[j++] = t[--i];
    }
    out[j] = 0;
}

/* ----------- Paridade 9 ----------- */
static int p9(u64 n) { return n ? (1 + (int)((n - 1) % 9)) : 0; }

/* ----------- ATA V1 ----------- */
static void u32_to_le(u32 v, unsigned char b[4]) {
    b[0] = v & 0xFF;
    b[1] = (v >> 8) & 0xFF;
    b[2] = (v >> 16) & 0xFF;
    b[3] = (v >> 24) & 0xFF;
}

void main() {
    const int AT_FDCWD = -100;
    const int O_WRONLY = 1;
    const int O_CREAT = 0100;
    const int O_TRUNC = 01000;

    g_hwif = rmr_hwif_bootstrap();

    s64 fd = sys_openat(AT_FDCWD, "ATA_OMEGA.bin", O_WRONLY | O_CREAT | O_TRUNC, 0600);

    u64 cpu_id = g_hwif->read_cpu_id_raw();
    u64 hw_sig64 = (cpu_id << 1) ^ 0x9E3779B97F4A7C15UL;

    char cpu_v20[24], sig_v20[24];
    v20(cpu_id, cpu_v20);
    v20(hw_sig64, sig_v20);

    raw_print("\n[RAFAELIA_OMEGA] ATA(V1) :: HWIF + 42\n");
    raw_print("BACKEND: ");
    raw_print(g_hwif->backend_name);
    raw_print("\n");
    raw_print("CPU_ID_RAW(V20): ");
    raw_print(cpu_v20);
    raw_print("\n");
    raw_print("HW_SIG64(V20): ");
    raw_print(sig_v20);
    raw_print("\n");
    raw_print("--------------------------------------------------------------\n");

    if (fd > 0) {
        mem_write((int)fd, "RFA\0", 4);

        unsigned char b[4];
        u32_to_le(1, b);
        mem_write((int)fd, (char*)b, 4);
        u32_to_le(24, b);
        mem_write((int)fd, (char*)b, 4);
        u32_to_le(42, b);
        mem_write((int)fd, (char*)b, 4);
        u32_to_le(0, b);
        mem_write((int)fd, (char*)b, 4);
        mem_write((int)fd, &hw_sig64, 8);
    }

    for (u64 k = 1; k <= 42; k++) {
        u64 cyc = g_hwif->read_time_raw();
        u64 par = (u64)p9(cyc + k);

        if (fd > 0) {
            mem_write((int)fd, &k, 8);
            mem_write((int)fd, &cyc, 8);
            mem_write((int)fd, &par, 8);
        }

        char cyc_v20[24];
        v20(cyc, cyc_v20);
        raw_print("[CHAVE:");
        char id[5];
        id[0] = (k / 100) + '0';
        id[1] = ((k / 10) % 10) + '0';
        id[2] = (k % 10) + '0';
        id[3] = 0;
        raw_print(id);
        raw_print("] [CYC:");
        raw_print(cyc_v20);
        raw_print("] [P:");
        char pc[2];
        pc[0] = (char)('0' + (int)par);
        pc[1] = 0;
        raw_print(pc);
        raw_print("] ");

        if (par == 9) {
            raw_print("SINGULARIDADE_OMEGA\n");
        } else if (par == 3 || par == 6) {
            raw_print("RESSONANCIA\n");
        } else {
            raw_print("FLUXO\n");
        }
    }

    if (fd > 0) {
        sys_fsync((int)fd);
        sys_close((int)fd);
    }
    raw_print("--------------------------------------------------------------\nOK: ATA_OMEGA.bin V1 gravada.\n");
}
