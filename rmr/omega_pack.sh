#!/data/data/com.termux/files/usr/bin/bash
set -e

# ---------------------------
# 1) KERNEL OMEGA (ARM64)
# ---------------------------
cat <<'EOK' > kernel_omega.S
/* ---------------------------------------------------------
 * RAFAELIA OMEGA - ARM64 NO-LIBC
 * sys_write / openat / close + cntvct_el0 + hw_sig
 * --------------------------------------------------------- */
.section .text
.global _start
.global sys_write
.global sys_open
.global sys_close
.global get_cycles
.global get_hw_id

_start:
    bl main
    mov x0, #0
    mov x8, #93             // exit (aarch64)
    svc #0

// x0=fd, x1=buf, x2=len
sys_write:
    mov x8, #64             // write
    svc #0
    ret

// openat: x0=dfd, x1=path, x2=flags, x3=mode
sys_open:
    mov x8, #56             // openat
    svc #0
    ret

// close: x0=fd
sys_close:
    mov x8, #57
    svc #0
    ret

get_cycles:
    mrs x0, cntvct_el0      // contador físico (se habilitado)
    ret

get_hw_id:
    // Em alguns kernels pode falhar (EL1). No seu, já está funcionando.
    mrs x0, midr_el1
    ret
EOK

# ---------------------------
# 2) CORE OMEGA (C no-libc)
# ---------------------------
cat <<'EOC' > rafaelia_core.c
typedef unsigned long u64;
typedef unsigned int  u32;

extern void sys_write(int fd, const char* buf, u64 len);
extern long sys_open(long dfd, const char* filename, long flags, long mode);
extern long sys_close(long fd);
extern u64 get_cycles();
extern u64 get_hw_id();

static u64 cstrlen(const char* s){ u64 n=0; while(s[n]) n++; return n; }
static void out(const char* s){ sys_write(1, s, cstrlen(s)); }

static int p9(u64 n){
    if(n==0) return 0;
    return 1 + ((n - 1) % 9);
}

static void v20(u64 n, char* outbuf){
    const char g[]="0123456789ABCDEFGHIJ";
    char tmp[32]; int i=0,j=0;
    if(n==0){ outbuf[0]='0'; outbuf[1]=0; return; }
    while(n){ tmp[i++]=g[n%20]; n/=20; }
    while(i) outbuf[j++]=tmp[--i];
    outbuf[j]=0;
}

static void w64(long fd, u64 v){
    sys_write((int)fd, (char*)&v, 8);
}
static void w32(long fd, u32 v){
    sys_write((int)fd, (char*)&v, 4);
}

void main(){
    // openat(AT_FDCWD=-100, "ATA_OMEGA.bin", O_WRONLY|O_CREAT|O_TRUNC, 0600)
    long fd = sys_open(-100, "ATA_OMEGA.bin", 01 | 0100 | 01000, 0600);

    u64 hw = get_hw_id();
    char hwv20[32]; v20(hw, hwv20);

    out("\n[RAFAELIA_OMEGA] + ATA BINARIA (no-libc)\n");
    out("HW_SIG(V20): "); out(hwv20); out("\n");
    out("--------------------------------------------------------------\n");

    // HEADER: magic "RFA\0" + hw_sig(u64)
    if(fd > 0){
        // magic: 0x00414652 = "RFA\0" little-endian em bytes: 52 46 41 00
        w32(fd, 0x00414652u);
        w64(fd, hw);
    }

    for(u64 k=1; k<=42; k++){
        u64 cyc = get_cycles();
        int par = p9(cyc + k);

        // RECORD: k(u64), cyc(u64), par(u64)
        if(fd > 0){
            w64(fd, k);
            w64(fd, cyc);
            w64(fd, (u64)par);
        }

        char kv20[16], cv20[32], pv20[8];
        v20(k, kv20);
        v20(cyc, cv20);
        v20((u64)par, pv20);

        out("[CHAVE:");
        if(k<10) out("0");
        char ks[3]; ks[0]=(char)('0'+(k/10)); ks[1]=(char)('0'+(k%10)); ks[2]=0; out(ks);

        out("] [CYC:"); out(cv20); out("] [P:"); out(pv20); out("] ");

        if(par==9) out("SINGULARIDADE_OMEGA\n");
        else if(par==3 || par==6) out("RESSONANCIA\n");
        else out("FLUXO\n");

        for(volatile u64 d=0; d < (par==9? 400000 : 1400000); d++);
    }

    if(fd > 0) sys_close(fd);

    out("--------------------------------------------------------------\n");
    out("OK: ATA_OMEGA.bin gravada.\n");
}
EOC

# ---------------------------
# 3) BUILD + RUN
# ---------------------------
cat <<'EOB' > build_omega_ata.sh
#!/data/data/com.termux/files/usr/bin/bash
set -e
echo "🔨 [OMEGA] Compilando (ARM64 / no-libc) + ATA..."
clang -c rafaelia_core.c -o core.o -ffreestanding -O3 -fno-stack-protector
clang -c kernel_omega.S -o kernel.o
clang kernel.o core.o -o rafaelia_omega_ata -nostdlib -Wl,-e,_start -pie
echo "🚀 [OMEGA] Executando..."
./rafaelia_omega_ata
echo "📦 ATA:"
ls -l ATA_OMEGA.bin || true
EOB
chmod +x build_omega_ata.sh

# ---------------------------
# 4) DECODER (PY)
# ---------------------------
cat <<'EOP' > ata_decode.py
#!/usr/bin/env python3
import struct

PATH="ATA_OMEGA.bin"
g="0123456789ABCDEFGHIJ"
def v20(n:int)->str:
    if n==0: return "0"
    s=[]
    while n>0:
        s.append(g[n%20]); n//=20
    return "".join(reversed(s))

with open(PATH,"rb") as f:
    magic=f.read(4)
    if magic!=b"RFA\0":
        raise SystemExit(f"magic errado: {magic!r}")
    sig,=struct.unpack("<Q", f.read(8))
    data=f.read()

recs=len(data)//24
print(f"[OK] magic=RFA\\0  hw_sig=0x{sig:016x}  hw_sig_v20={v20(sig)}  recs={recs}")

for i in range(recs):
    k,cyc,par=struct.unpack_from("<QQQ", data, i*24)
    print(f"[{i+1:02d}] k={k:02d}  cyc=0x{cyc:016x}  cyc_v20={v20(cyc)}  p={par}")
EOP
chmod +x ata_decode.py

echo "✅ Gerado: kernel_omega.S, rafaelia_core.c, build_omega_ata.sh, ata_decode.py"
echo "➡️ Rode agora: ./build_omega_ata.sh && ./ata_decode.py | head -n 60"
