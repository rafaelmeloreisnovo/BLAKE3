#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail

# ------------------------------------------------------------
# RAFAELIA OMEGA - ALL-IN-ONE (ARM64 / Termux)
# Gera: kernel_omega.S, rafaelia_core.c, build_omega.sh
# ------------------------------------------------------------

cat <<'ASM' > kernel_omega.S
/* ---------------------------------------------------------
 * RAFAELIA OMEGA KERNEL - ARM64 (AArch64)
 * Syscalls: write/exit
 * Auto-ID: MIDR_EL1 + MPIDR_EL1
 * Clock: CNTVCT_EL0 (cycle counter)
 * --------------------------------------------------------- */
.section .text
.global _start
.global sys_write
.global sys_exit
.global get_hw_sig
.global get_cycles

_start:
    bl main                // chama main() em C
    mov x0, #0
    mov x8, #93            // exit
    svc #0

// x0=fd, x1=buf, x2=len
sys_write:
    mov x8, #64            // write
    svc #0
    ret

sys_exit:
    mov x8, #93
    svc #0
    ret

// retorna em x0 uma assinatura combinada (MIDR + MPIDR)
get_hw_sig:
    mrs x0, midr_el1       // CPU Main ID
    mrs x1, mpidr_el1      // affinity / core topology
    eor x0, x0, x1, lsl #1 // mistura simples (determinística)
    ret

// retorna em x0 o contador de ciclos
get_cycles:
    mrs x0, cntvct_el0
    ret
ASM

cat <<'C' > rafaelia_core.c
/* ---------------------------------------------------------
   RAFAELIA OMEGA CORE - C (NO-LIBC / FREESTANDING)
   - Auto-ID (hw signature) em Base20 (BITRAF V20)
   - 42 chaves, paridade 9, leitura de ciclos
   --------------------------------------------------------- */
typedef unsigned long u64;

extern void sys_write(int fd, const char* buf, u64 len);
extern void sys_exit(int code);
extern u64  get_hw_sig(void);
extern u64  get_cycles(void);

static inline u64 c_strlen(const char* s){
    u64 n = 0; while(s[n]) n++; return n;
}

static inline void print(const char* s){
    sys_write(1, s, c_strlen(s));
}

static void to_v20(u64 n, char* out){
    const char g[] = "0123456789ABCDEFGHIJ";
    char tmp[32];
    int i = 0, j = 0;
    if(n == 0){ out[0]='0'; out[1]='\0'; return; }
    while(n > 0 && i < (int)sizeof(tmp)){
        tmp[i++] = g[n % 20];
        n /= 20;
    }
    while(i > 0) out[j++] = tmp[--i];
    out[j] = '\0';
}

static int p9(u64 n){
    if(n == 0) return 0;
    return 1 + (int)((n - 1) % 9);
}

static void print_u2(int v){
    char s[3];
    s[0] = (char)('0' + (v/10));
    s[1] = (char)('0' + (v%10));
    s[2] = '\0';
    print(s);
}

void main(void){
    u64 sig = get_hw_sig();
    char sig_v20[40];
    to_v20(sig, sig_v20);

    print("\n\x1b[1;32m[RAFAELIA_OMEGA] ARM64 NO-LIBC :: AUTO-ID + 42\x1b[0m\n");
    print("HW_SIG(V20): "); print(sig_v20); print("\n");
    print("--------------------------------------------------------------\n");

    for(int k=1; k<=42; k++){
        u64 t = get_cycles();
        int par = p9((u64)k + (t & 0xFFFF));

        char t_v20[40];
        to_v20(t, t_v20);

        print("[CHAVE:");
        print_u2(k);
        print("] [CYC:");
        // reduz um pouco só pra ficar legível, sem perder identidade
        // (a identidade completa segue no `t_v20` porque é V20 do u64)
        print(t_v20);
        print("] [P:");
        char pc[2] = { (char)('0' + par), '\0' };
        print(pc);
        print("] ");

        if(par == 9){
            print("\x1b[1;33mSINGULARIDADE_OMEGA\x1b[0m");
        } else if(par == 3 || par == 6){
            print("\x1b[1;36mRESSONANCIA\x1b[0m");
        } else {
            print("\x1b[0;90mFLUXO\x1b[0m");
        }
        print("\n");

        // delay bruto (sem nanosleep)
        for(volatile u64 d=0; d < (par==9 ? 300000 : 1200000); d++){}
    }

    print("--------------------------------------------------------------\n");
    print("\x1b[1;35mOK: 42 chaves + Auto-ID consagrados.\x1b[0m\n\n");
    sys_exit(0);
}
C

cat <<'BUILD' > build_omega.sh
#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
echo "🔨 [OMEGA] Compilando (ARM64 / no-libc)..."

# C freestanding
clang -c rafaelia_core.c -o core.o \
  -ffreestanding -fno-stack-protector -O2

# ASM ARM64
clang -c kernel_omega.S -o kernel.o

# Link no-libc, entrypoint _start, PIE (Android)
clang kernel.o core.o -o rafaelia_omega \
  -nostdlib -Wl,-e,_start -pie

echo "🚀 [OMEGA] Executando..."
./rafaelia_omega
BUILD

chmod +x build_omega.sh
echo "✅ Gerado: kernel_omega.S, rafaelia_core.c, build_omega.sh"
echo "➡️  Rode agora: ./build_omega.sh"
