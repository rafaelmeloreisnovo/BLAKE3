/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Linux/Termux adapter: direct syscalls, no libc, no heap.
 * This file is intentionally outside the pure core.
 */
#include "../../include/rmr_fc16.h"

typedef __INT64_TYPE__ s64;
typedef __INT32_TYPE__ s32;
typedef __INTPTR_TYPE__ slong;
typedef __UINTPTR_TYPE__ ulong;

#if defined(__x86_64__)
#define NR_READ 0
#define NR_WRITE 1
#define NR_CLOSE 3
#define NR_CLOCK_GETTIME 228
#define NR_GETDENTS64 217
#define NR_OPENAT 257
#define NR_STATX 332
#define NR_EXIT 60
#elif defined(__aarch64__)
#define NR_OPENAT 56
#define NR_CLOSE 57
#define NR_GETDENTS64 61
#define NR_READ 63
#define NR_WRITE 64
#define NR_EXIT 93
#define NR_CLOCK_GETTIME 113
#define NR_STATX 291
#elif defined(__arm__)
#define NR_EXIT 1
#define NR_READ 3
#define NR_WRITE 4
#define NR_CLOSE 6
#define NR_GETDENTS64 217
#define NR_CLOCK_GETTIME 263
#define NR_OPENAT 322
#define NR_STATX 397
#else
#error "Termux adapter supports x86_64, aarch64 and armv7"
#endif

#define AT_FDCWD (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64
#define O_TRUNC 512
#define O_DIRECTORY 65536
#define STATX_BASIC_STATS 0x7ffu
#define S_IFMT 0170000u
#define S_IFREG 0100000u
#define S_IFDIR 0040000u
#define S_IFLNK 0120000u
#define MODE_0600 0600u
#define MAX_DEPTH 8u
#define MAX_ENTRIES 128u
#define MAX_NAME 256u
#define MAX_PATH 768u
#define IO_BYTES 16384u

struct k_timespec { slong sec; slong nsec; };
struct statx_timestamp { s64 tv_sec; rmr_fc16_u32 tv_nsec; s32 reserved; };
struct k_statx {
    rmr_fc16_u32 mask, blksize;
    rmr_fc16_u64 attributes;
    rmr_fc16_u32 nlink, uid, gid;
    rmr_fc16_u16 mode, spare0;
    rmr_fc16_u64 ino, size, blocks, attributes_mask;
    struct statx_timestamp atime, btime, ctime, mtime;
    rmr_fc16_u32 rdev_major, rdev_minor, dev_major, dev_minor;
    rmr_fc16_u64 mnt_id;
    rmr_fc16_u32 dio_mem_align, dio_offset_align;
    rmr_fc16_u64 spare3[12];
};
struct linux_dirent64 { rmr_fc16_u64 ino; s64 off; rmr_fc16_u16 reclen; rmr_fc16_u8 type; char name[]; };
typedef struct scan_entry { rmr_fc16_u8 type; char name[MAX_NAME]; } scan_entry;

static scan_entry g_entries[MAX_DEPTH][MAX_ENTRIES];
static char g_paths[MAX_DEPTH + 1u][MAX_PATH];
static rmr_fc16_u8 g_io[IO_BYTES];
static rmr_fc16_u8 g_seal[RMR_FC16_SEAL_BYTES];
static rmr_fc16_u8 g_zip[RMR_FC16_SEAL_BYTES + RMR_FC16_ZIP_OVERHEAD];
static rmr_fc16_u8 g_digest[76];
static rmr_fc16_u8 g_warm_a[256], g_warm_b[256];

static slong kcall6(slong nr,slong a,slong b,slong c,slong d,slong e,slong f){
#if defined(__x86_64__)
    register slong r10 __asm__("r10")=d;register slong r8 __asm__("r8")=e;register slong r9 __asm__("r9")=f;slong ret;__asm__ volatile("syscall":"=a"(ret):"a"(nr),"D"(a),"S"(b),"d"(c),"r"(r10),"r"(r8),"r"(r9):"rcx","r11","memory");return ret;
#elif defined(__aarch64__)
    register slong x0 __asm__("x0")=a,x1 __asm__("x1")=b,x2 __asm__("x2")=c,x3 __asm__("x3")=d,x4 __asm__("x4")=e,x5 __asm__("x5")=f,x8 __asm__("x8")=nr;__asm__ volatile("svc #0":"+r"(x0):"r"(x1),"r"(x2),"r"(x3),"r"(x4),"r"(x5),"r"(x8):"memory","cc");return x0;
#else
    register slong r0 __asm__("r0")=a,r1 __asm__("r1")=b,r2 __asm__("r2")=c,r3 __asm__("r3")=d,r4 __asm__("r4")=e,r5 __asm__("r5")=f,r7 __asm__("r7")=nr;__asm__ volatile("svc #0":"+r"(r0):"r"(r1),"r"(r2),"r"(r3),"r"(r4),"r"(r5),"r"(r7):"memory","cc","r6","r8","r9","r10","r11","r12");return r0;
#endif
}
static slong kopenat(slong d,const char*p,slong fl,slong m){return kcall6(NR_OPENAT,d,(slong)p,fl,m,0,0);}
static slong kread(slong fd,void*p,rmr_fc16_usize n){return kcall6(NR_READ,fd,(slong)p,(slong)n,0,0,0);}
static slong kwrite(slong fd,const void*p,rmr_fc16_usize n){return kcall6(NR_WRITE,fd,(slong)p,(slong)n,0,0,0);}
static slong kclose(slong fd){return kcall6(NR_CLOSE,fd,0,0,0,0,0);}
static slong kgetdents(slong fd,void*p,rmr_fc16_usize n){return kcall6(NR_GETDENTS64,fd,(slong)p,(slong)n,0,0,0);}
static slong kstatx(slong fd,const char*p,rmr_fc16_u32 flags,struct k_statx *s){return kcall6(NR_STATX,fd,(slong)p,flags,STATX_BASIC_STATS,(slong)s,0);}

static rmr_fc16_usize slen(const char*s){rmr_fc16_usize n=0u;if(s)while(s[n])n++;return n;}
static int scmp(const char*a,const char*b){rmr_fc16_usize i=0u;while(a[i]&&a[i]==b[i])i++;return (int)(unsigned char)a[i]-(int)(unsigned char)b[i];}
static void scopy(char*d,const char*s,rmr_fc16_usize cap){rmr_fc16_usize i=0u;if(!cap)return;while(i+1u<cap&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static int is_dot(const char*n){return n[0]=='.'&&((n[1]==0)||(n[1]=='.'&&n[2]==0));}
static int is_generated(const char*n){return scmp(n,"custody16.bin")==0||scmp(n,"custody16.zip")==0||scmp(n,"custody16.digest")==0;}
static void out(const char*s){rmr_fc16_usize n=slen(s),o=0u;while(o<n){slong w=kwrite(1,s+o,n-o);if(w<=0)return;o+=(rmr_fc16_usize)w;}}
static void out_hex32(rmr_fc16_u32 v){static const char h[]="0123456789abcdef";char b[11];b[0]='0';b[1]='x';for(int i=9;i>=2;i--){b[i]=h[v&15u];v>>=4u;}b[10]='\n';(void)kwrite(1,b,11u);}
static void out_u64(rmr_fc16_u64 v){char b[32];rmr_fc16_u32 i=31u;b[i--]='\n';if(!v)b[i--]='0';while(v&&i<31u){b[i--]=(char)('0'+v%10u);v/=10u;}(void)kwrite(1,b+i+1u,31u-i);}

static rmr_fc16_u64 now_ns(void){struct k_timespec t;if(kcall6(NR_CLOCK_GETTIME,1,(slong)&t,0,0,0,0)<0)return 0u;return (rmr_fc16_u64)t.sec*1000000000ull+(rmr_fc16_u64)t.nsec;}
static rmr_fc16_u64 hw_stamp(void){
#if defined(__x86_64__)
    rmr_fc16_u32 a=0u,b=0u,c=0u,d=0u;__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(0u),"c"(0u));rmr_fc16_u64 x=((rmr_fc16_u64)b<<32u)^c^((rmr_fc16_u64)d<<1u);__asm__ volatile("cpuid":"=a"(a),"=b"(b),"=c"(c),"=d"(d):"a"(1u),"c"(0u));return x^a^((rmr_fc16_u64)c<<32u)^d;
#elif defined(__aarch64__)
    rmr_fc16_u64 f=0u;__asm__ volatile("mrs %0,cntfrq_el0":"=r"(f));return 0x4136340000000000ull^f;
#else
    return 0x41524d3332000000ull;
#endif
}
static rmr_fc16_u32 read_temp(void){
#ifndef RMR_FC16_THERMAL_PATH
#define RMR_FC16_THERMAL_PATH "/sys/class/thermal/thermal_zone0/temp"
#endif
    slong fd=kopenat(AT_FDCWD,RMR_FC16_THERMAL_PATH,O_RDONLY,0);if(fd<0)return 0u;char b[32];slong n=kread(fd,b,sizeof(b));kclose(fd);if(n<=0)return 0u;rmr_fc16_u32 v=0u;for(slong i=0;i<n;i++){if(b[i]<'0'||b[i]>'9')break;v=v*10u+(rmr_fc16_u32)(b[i]-'0');}return v;
}

static int make_path(char*out,const char*prefix,const char*name){rmr_fc16_usize a=slen(prefix),b=slen(name);if(a+b+2u>MAX_PATH)return -1;rmr_fc16_usize o=0u;for(;o<a;o++)out[o]=prefix[o];if(a){out[o++]='/';}for(rmr_fc16_usize i=0u;i<b;i++)out[o++]=name[i];out[o]=0;return 0;}
static void copy_entry(scan_entry*d,const scan_entry*s){d->type=s->type;for(rmr_fc16_u32 i=0u;i<MAX_NAME;++i)d->name[i]=s->name[i];}
static void sort_entries(scan_entry *e,rmr_fc16_u32 n){for(rmr_fc16_u32 i=1u;i<n;i++){scan_entry key;copy_entry(&key,&e[i]);rmr_fc16_u32 j=i;while(j&&scmp(e[j-1u].name,key.name)>0){copy_entry(&e[j],&e[j-1u]);j--;}copy_entry(&e[j],&key);}}

static rmr_fc16_u32 count_file(slong dirfd,const char*name,rmr_fc16_u64 *size_out){slong fd=kopenat(dirfd,name,O_RDONLY,0);if(fd<0)return RMR_FC16_ERR_STATE;rmr_fc16_u64 total=0u;for(;;){slong n=kread(fd,g_io,sizeof(g_io));if(n<0){kclose(fd);return RMR_FC16_ERR_STATE;}if(n==0)break;total+=(rmr_fc16_u64)n;}kclose(fd);*size_out=total;return RMR_FC16_OK;}

static rmr_fc16_u32 scan_file(rmr_fc16_context*c,slong dirfd,const char*name,const char*path,rmr_fc16_u64 size){
    slong fd=kopenat(dirfd,name,O_RDONLY,0);if(fd<0)return RMR_FC16_ERR_STATE;rmr_fc16_u32 rc=rmr_fc16_entry_begin(c,(const rmr_fc16_u8*)path,(rmr_fc16_u32)slen(path),size,RMR_FC16_ENTRY_REGULAR);if(rc){kclose(fd);return rc;}rmr_fc16_u64 seen=0u;for(;;){slong n=kread(fd,g_io,sizeof(g_io));if(n<0){kclose(fd);return RMR_FC16_ERR_STATE;}if(n==0)break;seen+=(rmr_fc16_u64)n;rc=rmr_fc16_entry_data(c,g_io,(rmr_fc16_usize)n);if(rc){kclose(fd);return rc;}}kclose(fd);if(seen!=size)return RMR_FC16_ERR_SIZE;return rmr_fc16_entry_end(c);
}

static rmr_fc16_u32 scan_dir(rmr_fc16_context*c,slong dirfd,const char*prefix,rmr_fc16_u32 depth){
    if(depth>=MAX_DEPTH)return RMR_FC16_ERR_CAPACITY;scan_entry *list=g_entries[depth];rmr_fc16_u32 count=0u;for(;;){slong n=kgetdents(dirfd,g_io,sizeof(g_io));if(n<0)return RMR_FC16_ERR_STATE;if(n==0)break;rmr_fc16_usize off=0u;while(off<(rmr_fc16_usize)n){struct linux_dirent64*d=(struct linux_dirent64*)(void*)(g_io+off);if(d->reclen<20u)return RMR_FC16_ERR_INTEGRITY;if(!is_dot(d->name)&&!is_generated(d->name)){if(count>=MAX_ENTRIES)return RMR_FC16_ERR_CAPACITY;scopy(list[count].name,d->name,MAX_NAME);list[count].type=d->type;count++;}off+=d->reclen;}}
    sort_entries(list,count);
    for(rmr_fc16_u32 i=0u;i<count;i++){
        struct k_statx st;slong sx=kstatx(dirfd,list[i].name,AT_SYMLINK_NOFOLLOW,&st);if(make_path(g_paths[depth+1u],prefix,list[i].name)<0)return RMR_FC16_ERR_CAPACITY;rmr_fc16_u32 type=0u;rmr_fc16_u64 size=0u;if(sx>=0){type=(rmr_fc16_u32)st.mode&S_IFMT;size=st.size;}else{if(list[i].type==4u)type=S_IFDIR;else if(list[i].type==8u)type=S_IFREG;else if(list[i].type==10u)type=S_IFLNK;else return RMR_FC16_ERR_STATE;if(type==S_IFREG){rmr_fc16_u32 cr=count_file(dirfd,list[i].name,&size);if(cr)return cr;}}
        if(type==S_IFDIR){rmr_fc16_u32 rc=rmr_fc16_entry_begin(c,(const rmr_fc16_u8*)g_paths[depth+1u],(rmr_fc16_u32)slen(g_paths[depth+1u]),0u,RMR_FC16_ENTRY_DIRECTORY);if(rc)return rc;if((rc=rmr_fc16_entry_end(c)))return rc;slong child=kopenat(dirfd,list[i].name,O_RDONLY|O_DIRECTORY,0);if(child<0)return RMR_FC16_ERR_STATE;rc=scan_dir(c,child,g_paths[depth+1u],depth+1u);kclose(child);if(rc)return rc;
        }else if(type==S_IFREG){rmr_fc16_u32 rc=scan_file(c,dirfd,list[i].name,g_paths[depth+1u],size);if(rc)return rc;
        }else if(type==S_IFLNK){rmr_fc16_u32 rc=rmr_fc16_entry_begin(c,(const rmr_fc16_u8*)g_paths[depth+1u],(rmr_fc16_u32)slen(g_paths[depth+1u]),0u,RMR_FC16_ENTRY_SYMLINK_SKIPPED);if(rc)return rc;if((rc=rmr_fc16_entry_end(c)))return rc;}
    }
    return RMR_FC16_OK;
}

static rmr_fc16_u32 snapshot_root(const char*root,rmr_fc16_context*c){slong fd=kopenat(AT_FDCWD,root,O_RDONLY|O_DIRECTORY,0);if(fd<0)return RMR_FC16_ERR_STATE;g_paths[0][0]=0;rmr_fc16_u32 rc=scan_dir(c,fd,"",0u);kclose(fd);return rc;}
static rmr_fc16_u32 write_file(const char*path,const rmr_fc16_u8*p,rmr_fc16_usize n){slong fd=kopenat(AT_FDCWD,path,O_WRONLY|O_CREAT|O_TRUNC,MODE_0600);if(fd<0)return RMR_FC16_ERR_STATE;rmr_fc16_usize o=0u;while(o<n){slong w=kwrite(fd,p+o,n-o);if(w<=0){kclose(fd);return RMR_FC16_ERR_STATE;}o+=(rmr_fc16_usize)w;}kclose(fd);return RMR_FC16_OK;}

static void digest_make(const rmr_fc16_u8 *seal,rmr_fc16_usize sl,const rmr_fc16_u8 *zip,rmr_fc16_usize zl){static const rmr_fc16_u8 m[8]={'R','F','C','1','6','D','1',0};for(rmr_fc16_u32 i=0u;i<8u;i++)g_digest[i]=m[i];rmr_fc16_sha256_ctx c;rmr_fc16_sha256_init(&c);rmr_fc16_sha256_update(&c,seal,sl);rmr_fc16_sha256_final(&c,g_digest+8u);rmr_fc16_sha256_init(&c);rmr_fc16_sha256_update(&c,zip,zl);rmr_fc16_sha256_final(&c,g_digest+40u);rmr_fc16_u32 crc=rmr_fc16_crc32c_finish(rmr_fc16_crc32c_update(0xffffffffu,g_digest,72u));g_digest[72]=(rmr_fc16_u8)crc;g_digest[73]=(rmr_fc16_u8)(crc>>8u);g_digest[74]=(rmr_fc16_u8)(crc>>16u);g_digest[75]=(rmr_fc16_u8)(crc>>24u);}

int rmr_fc16_termux_entry(ulong *stack){
    slong argc=(slong)stack[0];char **argv=(char**)&stack[1];const char*root=(argc>1)?argv[1]:".";for(rmr_fc16_u32 i=0u;i<256u;i++){g_warm_a[i]=(rmr_fc16_u8)i;g_warm_b[i]=(rmr_fc16_u8)(255u-i);}const rmr_fc16_u32 warm=3u;rmr_fc16_warmup(g_warm_a,g_warm_b,warm);
    rmr_fc16_u32 tb=read_temp();rmr_fc16_u32 limit=tb?85000u:0u,delta=tb?5000u:0u;rmr_fc16_u64 hw=hw_stamp();rmr_fc16_context ca,cb;rmr_fc16_snapshot sa,sb;rmr_fc16_init(&ca,hw,tb,limit,delta);rmr_fc16_init(&cb,hw,tb,limit,delta);
    rmr_fc16_u64 t0=now_ns();rmr_fc16_u32 rc=snapshot_root(root,&ca);rmr_fc16_u64 t1=now_ns();if(rc){out("FC16_SCAN_A_FAIL\n");return (int)rc;}rc=snapshot_root(root,&cb);rmr_fc16_u64 t2=now_ns();if(rc){out("FC16_SCAN_B_FAIL\n");return (int)rc;}rmr_fc16_u32 ta=read_temp();if(!tb)ta=0u;rmr_fc16_u32 ra=rmr_fc16_finalize(&ca,ta,&sa),rb=rmr_fc16_finalize(&cb,ta,&sb);if(ra==RMR_FC16_ERR_THERMAL||rb==RMR_FC16_ERR_THERMAL){out("FC16_THERMAL_REJECT\n");return RMR_FC16_ERR_THERMAL;}if(ra||rb){out("FC16_FINALIZE_FAIL\n");return RMR_FC16_ERR_STATE;}if(!rmr_fc16_snapshot_equal(&sa,&sb)){out("FC16_SNAPSHOT_MISMATCH\n");return RMR_FC16_ERR_INTEGRITY;}
    sa.flags|=RMR_FC16_FLAG_SNAPSHOT_MATCH;sa.warmup_rounds=warm;sa.benchmark_samples=2u;sa.benchmark_a_ns=t1-t0;sa.benchmark_b_ns=t2-t1;rmr_fc16_usize seal_len=0u,zip_len=0u;if(rmr_fc16_seal(&sa,g_seal,sizeof(g_seal),&seal_len))return RMR_FC16_ERR_CAPACITY;if(rmr_fc16_verify_seal(g_seal,seal_len))return RMR_FC16_ERR_INTEGRITY;if(rmr_fc16_zip_store(g_seal,seal_len,g_zip,sizeof(g_zip),&zip_len))return RMR_FC16_ERR_CAPACITY;if(rmr_fc16_zip_verify_roundtrip(g_zip,zip_len,g_seal,seal_len))return RMR_FC16_ERR_INTEGRITY;digest_make(g_seal,seal_len,g_zip,zip_len);if(write_file("custody16.bin",g_seal,seal_len)||write_file("custody16.zip",g_zip,zip_len)||write_file("custody16.digest",g_digest,sizeof(g_digest)))return RMR_FC16_ERR_STATE;
    out("FC16_OK\nroot_crc32c=");out_hex32(sa.root_crc32c);out("scan_a_ns=");out_u64(sa.benchmark_a_ns);out("scan_b_ns=");out_u64(sa.benchmark_b_ns);out("entries=");out_u64(sa.entry_count);out("bytes=");out_u64(sa.total_bytes);out("seal_bytes=");out_u64(seal_len);out("zip_bytes=");out_u64(zip_len);out("digest_bytes=");out_u64(sizeof(g_digest));return 0;
}
