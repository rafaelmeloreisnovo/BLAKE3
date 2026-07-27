/* Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR. */
#include "../include/rmr_fc16.h"

#define ROR32(x,n) (((x) >> (n)) | ((x) << (32u-(n))))
#define CH(x,y,z) (((x)&(y)) ^ (~(x)&(z)))
#define MAJ(x,y,z) (((x)&(y)) ^ ((x)&(z)) ^ ((y)&(z)))
#define E0(x) (ROR32((x),2u)^ROR32((x),13u)^ROR32((x),22u))
#define E1(x) (ROR32((x),6u)^ROR32((x),11u)^ROR32((x),25u))
#define S0(x) (ROR32((x),7u)^ROR32((x),18u)^((x)>>3u))
#define S1(x) (ROR32((x),17u)^ROR32((x),19u)^((x)>>10u))

static const rmr_fc16_u32 k256[64] = {
0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u};

static void block(rmr_fc16_sha256_ctx *c, const rmr_fc16_u8 b[64]) {
    rmr_fc16_u32 w[64];
    for (rmr_fc16_u32 i=0u;i<16u;++i) w[i]=((rmr_fc16_u32)b[i*4u]<<24u)|((rmr_fc16_u32)b[i*4u+1u]<<16u)|((rmr_fc16_u32)b[i*4u+2u]<<8u)|b[i*4u+3u];
    for (rmr_fc16_u32 i=16u;i<64u;++i) w[i]=S1(w[i-2u])+w[i-7u]+S0(w[i-15u])+w[i-16u];
    rmr_fc16_u32 a=c->h[0],bb=c->h[1],cc=c->h[2],d=c->h[3],e=c->h[4],f=c->h[5],g=c->h[6],h=c->h[7];
    for (rmr_fc16_u32 i=0u;i<64u;++i){rmr_fc16_u32 t1=h+E1(e)+CH(e,f,g)+k256[i]+w[i];rmr_fc16_u32 t2=E0(a)+MAJ(a,bb,cc);h=g;g=f;f=e;e=d+t1;d=cc;cc=bb;bb=a;a=t1+t2;}
    c->h[0]+=a;c->h[1]+=bb;c->h[2]+=cc;c->h[3]+=d;c->h[4]+=e;c->h[5]+=f;c->h[6]+=g;c->h[7]+=h;
}

void rmr_fc16_sha256_init(rmr_fc16_sha256_ctx *c){
    if(!c)return;c->h[0]=0x6a09e667u;c->h[1]=0xbb67ae85u;c->h[2]=0x3c6ef372u;c->h[3]=0xa54ff53au;c->h[4]=0x510e527fu;c->h[5]=0x9b05688cu;c->h[6]=0x1f83d9abu;c->h[7]=0x5be0cd19u;c->bit_len=0u;c->buffer_len=0u;
}
void rmr_fc16_sha256_update(rmr_fc16_sha256_ctx *c,const rmr_fc16_u8 *p,rmr_fc16_usize n){
    if(!c||(!p&&n))return;c->bit_len+=(rmr_fc16_u64)n*8u;while(n--){c->buffer[c->buffer_len++]=*p++;if(c->buffer_len==64u){block(c,c->buffer);c->buffer_len=0u;}}
}
void rmr_fc16_sha256_final(rmr_fc16_sha256_ctx *c,rmr_fc16_u8 out[32]){
    if(!c||!out)return;c->buffer[c->buffer_len++]=0x80u;while(c->buffer_len!=56u){if(c->buffer_len==64u){block(c,c->buffer);c->buffer_len=0u;}c->buffer[c->buffer_len++]=0u;}for(int i=7;i>=0;--i)c->buffer[c->buffer_len++]=(rmr_fc16_u8)(c->bit_len>>((rmr_fc16_u32)i*8u));block(c,c->buffer);for(rmr_fc16_u32 i=0u;i<8u;++i){out[i*4u]=(rmr_fc16_u8)(c->h[i]>>24u);out[i*4u+1u]=(rmr_fc16_u8)(c->h[i]>>16u);out[i*4u+2u]=(rmr_fc16_u8)(c->h[i]>>8u);out[i*4u+3u]=(rmr_fc16_u8)c->h[i];}
}
