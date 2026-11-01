/*
 * Host-only regression and adversarial tests for ZRF1.
 * The tested production core uses BLAKE3 from the fork's original c/.
 * No private ZIPRAF implementation is copied into this public test.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define MAX 3145728u
static uint8_t source[MAX], packed[MAX+46u], restored[MAX];
static uint32_t seed=0x4a17c129u;
static unsigned int checks=0u;
extern int rmr_zrf1_pack(const unsigned char*,unsigned int,unsigned char*,unsigned int,unsigned int*);
extern int rmr_zrf1_unpack(const unsigned char*,unsigned int,unsigned char*,unsigned int,unsigned int*);
extern int rmr_zrf1_digest(const unsigned char*,unsigned int,unsigned char[32]);
static uint32_t rnd(void) {
  seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;
}
static void verify(int cond,const char *why) {
  ++checks;
  if (!cond) {fprintf(stderr,"ZRF1_TEST_FAIL=%s\n",why);fflush(stderr);_Exit(1);}
}
static void roundtrip(unsigned n,unsigned mode) {
  unsigned packed_n=0u,out_n=0u,i;
  unsigned char digest[32];
  for(i=0;i<n;++i) {
    if(mode==0u)source[i]=0x5au;
    else if(mode==1u)source[i]=(unsigned char)(i&255u);
    else if(mode==2u)source[i]=(unsigned char)rnd();
    else source[i]=(unsigned char)((i/700u)%7u ? (i&255u):0u);
  }
  verify(rmr_zrf1_digest(source,n,digest)==0,"digest");
  verify(rmr_zrf1_pack(source,n,packed,sizeof(packed),&packed_n)==0,"pack");
  verify(packed_n>=46u && packed_n<=n+46u,"packed-cap");
  verify(rmr_zrf1_unpack(packed,packed_n,restored,sizeof(restored),&out_n)==0,"unpack");
  verify(out_n==n,"restored-len");
  verify(memcmp(source,restored,n)==0,"roundtrip");
  if(n!=0u) {
    unsigned char save=packed[packed_n-1];
    packed[packed_n-1]^=0x81u;
    verify(rmr_zrf1_unpack(packed,packed_n,restored,sizeof(restored),&out_n)!=0,"corruption");
    packed[packed_n-1]=save;
  }
  verify(rmr_zrf1_unpack(packed,packed_n-1u,restored,sizeof(restored),&out_n)!=0,"truncation");
  packed[4]^=0x01u;
  verify(rmr_zrf1_unpack(packed,packed_n,restored,sizeof(restored),&out_n)!=0,"version");
  packed[4]^=0x01u;
}
int main(void) {
  const unsigned lens[]={0u,1u,2u,3u,4u,63u,64u,127u,128u,129u,130u,131u,
    1023u,1024u,1025u,4096u,65535u,65536u,136u*1024u,1048576u,MAX};
  unsigned k,m,w=0u;unsigned char first[32];
  const unsigned char abc[3]={'a','b','c'};
  verify(rmr_zrf1_digest(abc,3u,first)==0,"abc-digest");
  /* Canonical BLAKE3 empty/abc KATs, compared in binary. */
  static const unsigned char kat_abc[32]={
    0x64,0x37,0xb3,0xac,0x38,0x46,0x51,0x33,0xff,0xb6,0x3b,0x75,0x27,0x3a,0x8d,0xb5,
    0x48,0xc5,0x58,0x46,0x5d,0x79,0xdb,0x03,0xfd,0x35,0x9c,0x6c,0xd5,0xbd,0x9d,0x85
  };
  static const unsigned char kat_empty[32]={
    0xaf,0x13,0x49,0xb9,0xf5,0xf9,0xa1,0xa6,0xa0,0x40,0x4d,0xea,0x36,0xdc,0xc9,0x49,
    0x9b,0xcb,0x25,0xc9,0xad,0xc1,0x12,0xb7,0xcc,0x9a,0x93,0xca,0xe4,0x1f,0x32,0x62
  };
  verify(memcmp(first,kat_abc,32)==0,"BLAKE3_ABC_KAT");
  verify(rmr_zrf1_digest(source,0u,first)==0,"BLAKE3_EMPTY");
  verify(memcmp(first,kat_empty,32)==0,"BLAKE3_EMPTY_KAT");
  for(k=0;k<sizeof(lens)/sizeof(lens[0]);++k)
    for(m=0;m<4u;++m) roundtrip(lens[k],m);
  verify(rmr_zrf1_pack(source,MAX+1u,packed,sizeof(packed),&w)!=0,"oversized-input");
  verify(rmr_zrf1_pack(source,1u,packed,1u,&w)!=0,"small-output");
  verify(rmr_zrf1_pack(source,1u,packed,sizeof(packed),&w)==0,"prepare-header");
  packed[6]=0xff;packed[7]=0xff;packed[8]=0xff;packed[9]=0xff;
  verify(rmr_zrf1_unpack(packed,w,restored,sizeof(restored),&k)!=0,"oversized-raw-header");
  puts("RMR_ZRF1_CORE_TEST=PASS");
  printf("CHECKS=%u\n",checks);
  puts("BLAKE3_KAT=PASS");
  puts("DEVICE_ANDROID=NOT_RUN");
  puts("RELEASE_SIGNATURE=TOKEN_VAZIO");
  return 0;
}
