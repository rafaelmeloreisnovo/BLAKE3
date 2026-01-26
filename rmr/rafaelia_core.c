typedef unsigned long u64;
typedef unsigned int  u32;

extern void sys_write(int fd, const char* buf, u64 len);
extern int  sys_openat(int dfd, const char* path, int flags, int mode);
extern int  sys_close(int fd);
extern int  sys_fsync(int fd);

extern u64 get_cntvct();
extern u64 get_midr();
extern u64 get_mpidr();

/* ----------- IO básico ----------- */
static void raw_print(const char* s) {
    u64 n=0; while(s[n]) n++;
    sys_write(1, s, n);
}
static void mem_write(int fd, const void* p, u64 n) {
    sys_write(fd, (const char*)p, n);
}

/* ----------- Base20 (0-9A-J) ----------- */
static void v20(u64 x, char out[24]) {
    const char g[]="0123456789ABCDEFGHIJ";
    char t[24]; int i=0, j=0;
    if (x==0) { out[0]='0'; out[1]=0; return; }
    while(x){ t[i++]=g[x%20]; x/=20; }
    while(i) out[j++]=t[--i];
    out[j]=0;
}

/* ----------- Paridade 9 ----------- */
static int p9(u64 n){ return n? (1 + (int)((n-1)%9)) : 0; }

/* ----------- ATA V1 ----------- */
/*
Header:
  magic[4]   = "RFA\0"
  version u32= 1
  rsize   u32= 24
  count   u32= 42
  flags   u32= 0
  hw_sig  u64= HW_SIG64
Records (count * 24):
  k   u64
  cyc u64
  p   u64
*/
static void u32_to_le(u32 v, unsigned char b[4]){
    b[0]=v&0xFF; b[1]=(v>>8)&0xFF; b[2]=(v>>16)&0xFF; b[3]=(v>>24)&0xFF;
}

void main(){
    // Flags (Linux/Android): O_WRONLY=1, O_CREAT=0100, O_TRUNC=01000
    const int AT_FDCWD = -100;
    const int O_WRONLY = 1;
    const int O_CREAT  = 0100;
    const int O_TRUNC  = 01000;

    int fd = sys_openat(AT_FDCWD, "ATA_OMEGA.bin", O_WRONLY|O_CREAT|O_TRUNC, 0600);

    u64 midr  = get_midr();
    u64 mpidr = get_mpidr();
    u64 hw_sig64 = (midr<<32) ^ (mpidr & 0xFFFFFFFFULL);

    char midr_v20[24], mpidr_v20[24], sig_v20[24];
    v20(midr, midr_v20);
    v20(mpidr, mpidr_v20);
    v20(hw_sig64, sig_v20);

    raw_print("\n[RAFAELIA_OMEGA] ATA(V1) :: AUTO-ID64 + 42\n");
    raw_print("MIDR(V20): "); raw_print(midr_v20); raw_print("\n");
    raw_print("MPIDR(V20): "); raw_print(mpidr_v20); raw_print("\n");
    raw_print("HW_SIG64(V20): "); raw_print(sig_v20); raw_print("\n");
    raw_print("--------------------------------------------------------------\n");

    // Header
    if (fd > 0) {
        mem_write(fd, "RFA\0", 4);

        unsigned char b[4];
        u32_to_le(1, b);   mem_write(fd, (char*)b, 4);    // version
        u32_to_le(24, b);  mem_write(fd, (char*)b, 4);    // rsize
        u32_to_le(42, b);  mem_write(fd, (char*)b, 4);    // count
        u32_to_le(0, b);   mem_write(fd, (char*)b, 4);    // flags
        mem_write(fd, &hw_sig64, 8);                      // hw_sig64
    }

    for (u64 k=1; k<=42; k++){
        u64 cyc = get_cntvct();
        u64 par = (u64)p9(cyc + k);

        // escreve record
        if (fd > 0){
            mem_write(fd, &k, 8);
            mem_write(fd, &cyc, 8);
            mem_write(fd, &par, 8);
        }

        // log
        char cyc_v20[24]; v20(cyc, cyc_v20);
        raw_print("[CHAVE:");
        char id[5]; id[0]=(k/100)+'0'; id[1]=((k/10)%10)+'0'; id[2]=(k%10)+'0'; id[3]=0;
        raw_print(id);
        raw_print("] [CYC:"); raw_print(cyc_v20);
        raw_print("] [P:");
        char pc[2]; pc[0]=(char)('0'+(int)par); pc[1]=0;
        raw_print(pc);
        raw_print("] ");

        if (par==9) raw_print("SINGULARIDADE_OMEGA\n");
        else if (par==3 || par==6) raw_print("RESSONANCIA\n");
        else raw_print("FLUXO\n");

        for(volatile u64 d=0; d<2000000; d++);
    }

    if (fd > 0) { sys_fsync(fd); sys_close(fd); }
    raw_print("--------------------------------------------------------------\nOK: ATA_OMEGA.bin V1 gravada.\n");
}
