/* Hosted test only. The tested core itself remains freestanding. */
#include "../include/rmr_fc16.h"
#include <stdio.h>

static int same64(const rmr_fc16_u64 *a,const rmr_fc16_u64 *b,rmr_fc16_usize n){for(rmr_fc16_usize i=0;i<n;i++)if(a[i]!=b[i])return 0;return 1;}
static int eq8(const rmr_fc16_u8 *a,const rmr_fc16_u8 *b,rmr_fc16_usize n){rmr_fc16_u8 d=0;for(rmr_fc16_usize i=0;i<n;i++)d|=a[i]^b[i];return d==0;}
int main(void){
    static const rmr_fc16_u8 kat[]="123456789";
    if(rmr_fc16_crc32c_finish(rmr_fc16_crc32c_update(0xffffffffu,kat,9u))!=0xe3069283u)return 20;
    if(rmr_fc16_crc32_ieee(kat,9u)!=0xcbf43926u)return 21;
    {static const rmr_fc16_u8 abc[]={'a','b','c'};static const rmr_fc16_u8 want[32]={0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};rmr_fc16_sha256_ctx sh;rmr_fc16_u8 got[32];rmr_fc16_sha256_init(&sh);rmr_fc16_sha256_update(&sh,abc,3u);rmr_fc16_sha256_final(&sh,got);if(!eq8(got,want,32u))return 22;}
    static const rmr_fc16_u8 path[]="matrix/input.bin";
    rmr_fc16_u8 data[256],wa[256],wb[256],seal[RMR_FC16_SEAL_BYTES],zip[RMR_FC16_SEAL_BYTES+RMR_FC16_ZIP_OVERHEAD];
    for(rmr_fc16_u32 i=0;i<256u;i++){data[i]=(rmr_fc16_u8)(i^(i>>3u));wa[i]=(rmr_fc16_u8)i;wb[i]=(rmr_fc16_u8)(255u-i);}
    rmr_fc16_warmup(wa,wb,3u);
    rmr_fc16_context c;rmr_fc16_snapshot s;rmr_fc16_init(&c,0x1122334455667788ull,42000u,85000u,5000u);
    if(rmr_fc16_entry_begin(&c,path,sizeof(path)-1u,256u,RMR_FC16_ENTRY_REGULAR))return 1;
    if(rmr_fc16_entry_data(&c,data,37u))return 2;if(rmr_fc16_entry_data(&c,data+37u,219u))return 3;if(rmr_fc16_entry_end(&c))return 4;if(rmr_fc16_finalize(&c,43000u,&s))return 5;
    rmr_fc16_usize sl=0,zl=0;if(rmr_fc16_seal(&s,seal,sizeof(seal),&sl))return 6;if(rmr_fc16_verify_seal(seal,sl))return 7;if(rmr_fc16_zip_store(seal,sl,zip,sizeof(zip),&zl))return 8;if(rmr_fc16_zip_verify_roundtrip(zip,zl,seal,sl))return 9;
    rmr_fc16_u64 a[8]={0,1,2,3,4,5,6,7},b[8]={0,1,0x22,3,4,0x55,6,7},copy[8];for(int i=0;i<8;i++)copy[i]=a[i];rmr_fc16_delta64 d[8];rmr_fc16_usize dn=0;if(rmr_fc16_delta64_build(a,b,8,d,8,&dn)||dn!=2)return 10;if(rmr_fc16_delta64_apply(copy,8,d,dn)||!same64(copy,b,8))return 11;
    printf("FC16_OK seal=%zu zip=%zu root=%08x flags=%08x deltas=%zu\n",(size_t)sl,(size_t)zl,s.root_crc32c,s.flags,(size_t)dn);return 0;
}
