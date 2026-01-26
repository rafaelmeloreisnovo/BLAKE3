/* ---------------------------------------------------------
   RAFAELIA_SYNC_OMEGA - C puro (no-libc / freestanding)
   - Lê ATA_OMEGA.bin
   - Mistura (ATA_CYC + CNTVCT + MIDR + seed) -> X
   - Paridade 3-6-9 com p9()
   - Log: 42 chaves
   --------------------------------------------------------- */
typedef unsigned long  u64;
typedef long           s64;

extern void sys_write(int fd, const char* buf, u64 len);
extern int  sys_openat(int dfd, const char* path, int flags, int mode);
extern s64  sys_read(int fd, void* buf, u64 len);
extern int  sys_close(int fd);

extern u64 rd_cntfrq();
extern u64 rd_cntvct();
extern u64 rd_midr();

static void print(const char* s){
    u64 n=0; while(s[n]) n++;
    sys_write(1, s, n);
}

static void print_nl(){ sys_write(1, "\n", 1); }

/* Base20 (BITRAF v20) */
static void v20(u64 n, char* out){
    static const char g[]="0123456789ABCDEFGHIJ";
    char tmp[64];
    int i=0, j=0;
    if(n==0){ out[0]='0'; out[1]=0; return; }
    while(n){ tmp[i++]=g[n%20]; n/=20; }
    while(i) out[j++]=tmp[--i];
    out[j]=0;
}

/* Paridade vorticial 9 (1..9) */
static int p9(u64 n){
    if(n==0) return 0;
    return 1 + (int)((n-1) % 9);
}

/* Xorshift64* (mix bruto, rápido, estável) */
static u64 mix64(u64 x){
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * 2685821657736338717ULL;
}

/* ATA format: magic[4]="RFA\0", hw_sig(u64 LE), 42*(k,u64 cyc,u64 p) */
struct AtaRec { u64 k, cyc, par; };

static int load_ata(const char* path, u64* hw_sig_out, u64* arena, int cap){
    // openat(AT_FDCWD=-100, path, O_RDONLY=0, mode=0)
    int fd = sys_openat(-100, path, 0, 0);
    if(fd < 0) return -1;

    unsigned char magic[4];
    if(sys_read(fd, magic, 4) != 4){ sys_close(fd); return -2; }
    if(!(magic[0]=='R' && magic[1]=='F' && magic[2]=='A' && magic[3]==0)){ sys_close(fd); return -3; }

    u64 hw_sig=0;
    if(sys_read(fd, &hw_sig, 8) != 8){ sys_close(fd); return -4; }
    *hw_sig_out = hw_sig;

    int loaded=0;
    while(loaded < cap){
        struct AtaRec r;
        s64 got = sys_read(fd, &r, 24);
        if(got != 24) break;
        arena[loaded++] = r.cyc;
    }
    sys_close(fd);
    return loaded;
}

static void print_hdr(const char* title){
    print("\n"); print(title); print("\n");
}

void main(){
    u64 cntfrq = rd_cntfrq();
    u64 cntvct0 = rd_cntvct();
    u64 midr = rd_midr();

    char b_cntfrq[64], b_cntvct[64], b_midr[64];
    v20(cntfrq, b_cntfrq);
    v20(cntvct0, b_cntvct);
    v20(midr & 0xffffffffULL, b_midr); // compacto p/ log

    print_hdr("[RAFAELIA_SYNC_OMEGA] ARM64 no-libc :: ATA + CLOCK + 3-6-9");
    print("CNTFRQ(V20): "); print(b_cntfrq); print_nl();
    print("CNTVCT(V20): "); print(b_cntvct); print_nl();
    print("MIDR (V20): ");  print(b_midr);  print_nl();
    print("--------------------------------------------------------------\n");

    // Carrega ATA
    u64 arena[42];
    u64 ata_hw_sig = 0;
    int loaded = load_ata("ATA_OMEGA.bin", &ata_hw_sig, arena, 42);

    // Prova forense (se ATA existir)
    if(loaded > 0){
        char hw20[64]; v20(ata_hw_sig, hw20);
        print("ATA_HW_SIG(V20): "); print(hw20); print_nl();
        print("ATA_LOADED: ");
        char ld[8];
        ld[0]=(char)((loaded/10)+'0');
        ld[1]=(char)((loaded%10)+'0');
        ld[2]=0;
        if(loaded < 10){ ld[0]='0'; ld[1]=(char)('0'+loaded); ld[2]=0; }
        print(ld); print_nl();

        char c0[64], c1[64], c2[64];
        v20(arena[0], c0);
        v20(arena[(loaded>1)?1:0], c1);
        v20(arena[(loaded>2)?2:0], c2);
        print("CYC0: "); print(c0); print_nl();
        print("CYC1: "); print(c1); print_nl();
        print("CYC2: "); print(c2); print_nl();
        print("--------------------------------------------------------------\n");
    } else {
        print("WARN: ATA_OMEGA.bin nao carregada (loaded<=0). SYNC em modo clock-only.\n");
        print("--------------------------------------------------------------\n");
    }

    // Seed corrigida (NUNCA 0xRFA inválido)
    // "RFA" em ASCII: 0x52 0x46 0x41 => 0x524641
    u64 seed = 0x524641ULL ^ (midr << 1) ^ (cntfrq << 7);

    // Loop 42
    for(int i=1; i<=42; i++){
        u64 t = rd_cntvct();
        u64 cyc = (loaded > 0) ? arena[(i-1) % loaded] : 0;
        u64 x = mix64( seed ^ t ^ (cyc * 0x9e3779b97f4a7c15ULL) ^ (u64)i );
        int p = p9(x);

        char bx[64];
        v20(x, bx);

        // CHAVE id (3 dígitos)
        char id[8];
        id[0] = (char)('0' + (i/100)%10);
        id[1] = (char)('0' + (i/10)%10);
        id[2] = (char)('0' + (i%10));
        id[3] = 0;

        char pc[4]; pc[0]=(char)('0'+p); pc[1]=0;

        print("[CHAVE:"); print(id); print("] [X:"); print(bx); print("] [P:"); print(pc); print("] ");

        if(p==9){
            print("SINGULARIDADE_OMEGA\n");
        } else if(p==3 || p==6){
            print("RESSONANCIA\n");
        } else {
            print("FLUXO\n");
        }

        // cadência (ajuste fino)
        for(volatile u64 d=0; d<350000; d++);
    }

    print("--------------------------------------------------------------\n");
    print("OK: SYNC concluído.\n");
}
