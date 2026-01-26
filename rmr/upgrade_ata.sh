#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail

cat <<'ASM' > kernel_omega.S
.section .text
.global _start
.global sys_write
.global sys_exit
.global sys_openat
.global sys_close
.global get_hw_sig
.global get_cycles

_start:
    bl main
    mov x0, #0
    mov x8, #93
    svc #0

sys_write:
    mov x8, #64
    svc #0
    ret

sys_exit:
    mov x8, #93
    svc #0
    ret

// openat(dirfd, pathname, flags, mode) -> x0=fd
sys_openat:
    mov x8, #56
    svc #0
    ret

sys_close:
    mov x8, #57
    svc #0
    ret

get_hw_sig:
    mrs x0, midr_el1
    mrs x1, mpidr_el1
    eor x0, x0, x1, lsl #1
    ret

get_cycles:
    mrs x0, cntvct_el0
    ret
ASM

cat <<'C' > rafaelia_core.c
typedef unsigned long u64;

extern void sys_write(int fd, const char* buf, u64 len);
extern void sys_exit(int code);
extern int  sys_openat(int dirfd, const char* path, int flags, int mode);
extern void sys_close(int fd);
extern u64  get_hw_sig(void);
extern u64  get_cycles(void);

static inline u64 c_strlen(const char* s){ u64 n=0; while(s[n]) n++; return n; }
static inline void print(const char* s){ sys_write(1, s, c_strlen(s)); }

static void to_v20(u64 n, char* out){
    const char g[]="0123456789ABCDEFGHIJ";
    char tmp[32]; int i=0,j=0;
    if(n==0){ out[0]='0'; out[1]='\0'; return; }
    while(n>0 && i<(int)sizeof(tmp)){ tmp[i++]=g[n%20]; n/=20; }
    while(i>0) out[j++]=tmp[--i];
    out[j]='\0';
}

static int p9(u64 n){ return (n==0)?0:1+(int)((n-1)%9); }

// Registro ATA (24 bytes): k(8) | cyc(8) | par(8)
struct AtaRec { u64 k; u64 cyc; u64 par; };

void main(void){
    u64 sig = get_hw_sig();
    char sig_v20[40]; to_v20(sig, sig_v20);

    // flags (octal compat): O_WRONLY=1, O_CREAT=0100, O_APPEND=02000
    int fd = sys_openat(-100, "ATA_OMEGA.bin", 01 | 0100 | 02000, 0666);

    print("\n\x1b[1;32m[RAFAELIA_OMEGA] + ATA BINARIA (no-libc)\x1b[0m\n");
    print("HW_SIG(V20): "); print(sig_v20); print("\n");

    // Grava cabeçalho binário: "RFA\0" + sig(u64)
    if(fd > 0){
        const char magic[4] = {'R','F','A','\0'};
        sys_write(fd, magic, 4);
        sys_write(fd, (const char*)&sig, 8);
    }

    print("--------------------------------------------------------------\n");

    for(int k=1; k<=42; k++){
        u64 cyc = get_cycles();
        int par = p9((u64)k + (cyc & 0xFFFF));

        char cyc_v20[40]; to_v20(cyc, cyc_v20);

        print("[CHAVE:");
        char ks[3]={ (char)('0'+k/10), (char)('0'+k%10), 0 };
        print(ks);
        print("] [CYC:"); print(cyc_v20); print("] [P:");
        char pc[2]={ (char)('0'+par), 0 };
        print(pc); print("] ");

        if(par==9) print("\x1b[1;33mSINGULARIDADE_OMEGA\x1b[0m\n");
        else if(par==3 || par==6) print("\x1b[1;36mRESSONANCIA\x1b[0m\n");
        else print("\x1b[0;90mFLUXO\x1b[0m\n");

        if(fd > 0){
            struct AtaRec r;
            r.k   = (u64)k;
            r.cyc = cyc;
            r.par = (u64)par;
            sys_write(fd, (const char*)&r, sizeof(r));
        }

        for(volatile u64 d=0; d < (par==9 ? 300000 : 1200000); d++){}
    }

    if(fd > 0) sys_close(fd);

    print("--------------------------------------------------------------\n");
    print("\x1b[1;35mOK: ATA_OMEGA.bin gravada.\x1b[0m\n");

    sys_exit(0);
}
C

cat <<'BUILD' > build_omega.sh
#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
echo "🔨 [OMEGA] Compilando (ARM64 / no-libc) + ATA..."
clang -c rafaelia_core.c -o core.o -ffreestanding -fno-stack-protector -O2
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_omega -nostdlib -Wl,-e,_start -pie
echo "🚀 [OMEGA] Executando..."
./rafaelia_omega
echo "📦 ATA:"
ls -l ATA_OMEGA.bin || true
BUILD

chmod +x build_omega.sh
./build_omega.sh
