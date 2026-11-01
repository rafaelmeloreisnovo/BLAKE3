/*
 * RMR ZIPRAF-FREE profile ZRF1, independent experimental lossless format.
 * Copyright (c) 2026 Rafael Melo Reis. Licensed under rmr/LICENSE_RMR.
 * BLAKE3 algorithm/implementation remains BLAKE3-team upstream-licensed.
 * Core: caller-owned buffers, no heap, no libc calls, bounded 3 MiB.
 * ZRF1 is NOT claimed compatible with private ZIPRAF formats.
 */
#include "blake3.h"
typedef __UINT8_TYPE__ z_u8;
typedef __UINT32_TYPE__ z_u32;
#define Z_MAX (3u * 1024u * 1024u)
#define Z_HDR 46u

static void z_copy(z_u8 *d, const z_u8 *s, z_u32 n) {
  z_u32 i; for (i = 0; i < n; ++i) d[i] = s[i];
}
static void z_put32(z_u8 *p, z_u32 v) {
  p[0]=(z_u8)v; p[1]=(z_u8)(v>>8);
  p[2]=(z_u8)(v>>16); p[3]=(z_u8)(v>>24);
}
static z_u32 z_get32(const z_u8 *p) {
  return (z_u32)p[0] | ((z_u32)p[1]<<8) |
         ((z_u32)p[2]<<16) | ((z_u32)p[3]<<24);
}
static void z_hash(const z_u8 *p, z_u32 n, z_u8 out[32]) {
  blake3_hasher h;
  static const z_u8 empty = 0;
  blake3_hasher_init(&h);
  blake3_hasher_update(&h, n ? p : &empty, n);
  blake3_hasher_finalize(&h, out, 32u);
}
static z_u32 z_run(const z_u8 *src, z_u32 n, z_u32 pos) {
  z_u32 m=1u;
  while (m < 130u && pos+m < n && src[pos+m] == src[pos]) ++m;
  return m;
}
/* Same deterministic traversal measures and emits the RLE stream. */
static z_u32 z_rle(const z_u8 *src, z_u32 n, z_u8 *dst) {
  z_u32 i=0u, o=0u, s, count, k;
  while (i < n) {
    count=z_run(src,n,i);
    if (count >= 4u) {
      if (dst) { dst[o]=(z_u8)(0x80u | (count-3u)); dst[o+1]=src[i]; }
      o+=2u; i+=count;
    } else {
      s=i; i+=count;
      while (i < n && i-s < 128u) {
        count=z_run(src,n,i);
        if (count >= 4u || i-s+count > 128u) break;
        i+=count;
      }
      count=i-s;
      if (dst) {
        dst[o]=(z_u8)(count-1u);
        for (k=0u;k<count;++k) dst[o+1u+k]=src[s+k];
      }
      o+=count+1u;
    }
  }
  return o;
}
int rmr_zrf1_digest(const z_u8 *src, z_u32 n, z_u8 out[32]) {
  if ((!src && n) || !out || n>Z_MAX) return -1;
  z_hash(src,n,out); return 0;
}
int rmr_zrf1_pack(const z_u8 *src, z_u32 n,
                  z_u8 *dst, z_u32 cap, z_u32 *written) {
  z_u32 encoded, payload;
  z_u8 mode;
  if ((!src && n) || !dst || !written || n>Z_MAX) return -1;
  encoded=z_rle(src,n,(z_u8*)0);
  mode=(z_u8)(encoded<n ? 1u : 0u);
  payload=mode?encoded:n;
  if (cap<Z_HDR || payload>cap-Z_HDR) return -2;
  dst[0]='Z';dst[1]='R';dst[2]='F';dst[3]='1';
  dst[4]=1u;dst[5]=mode;
  z_put32(dst+6,n);z_put32(dst+10,payload);
  z_hash(src,n,dst+14);
  if (mode) (void)z_rle(src,n,dst+Z_HDR);
  else if (n) z_copy(dst+Z_HDR,src,n);
  *written=Z_HDR+payload; return 0;
}
int rmr_zrf1_unpack(const z_u8 *src, z_u32 n,
                    z_u8 *dst, z_u32 cap, z_u32 *written) {
  z_u32 raw,plen,ip=Z_HDR,op=0u,k,count;
  z_u8 dig[32], mode,token;
  z_u32 diff=0u;
  if (!src || !dst || !written || n<Z_HDR) return -1;
  if (src[0]!='Z'||src[1]!='R'||src[2]!='F'||src[3]!='1'||src[4]!=1u)
    return -2;
  mode=src[5];
  if (mode>1u) return -2;
  raw=z_get32(src+6);plen=z_get32(src+10);
  if (raw>Z_MAX || raw>cap || plen!=n-Z_HDR) return -2;
  if (mode==0u) {
    if (plen!=raw) return -2;
    z_copy(dst,src+Z_HDR,raw);
    op=raw;
  } else {
    while (ip<n) {
      token=src[ip++];
      if (token&0x80u) {
        count=(z_u32)(token&0x7fu)+3u;
        if (ip>=n || count>raw-op) return -3;
        for (k=0;k<count;++k) dst[op+k]=src[ip];
        ++ip; op+=count;
      } else {
        count=(z_u32)token+1u;
        if (count>n-ip || count>raw-op) return -3;
        z_copy(dst+op,src+ip,count);
        ip+=count;op+=count;
      }
    }
    if (op!=raw) return -3;
  }
  z_hash(dst,raw,dig);
  for (k=0;k<32u;++k) diff |= (z_u32)(dig[k]^src[14u+k]);
  if (diff!=0u) return -4;
  *written=raw; return 0;
}

#ifndef RMR_ZRF_HOST_TEST
#include <jni.h>
/* Android-hosted JNI adapter; the three core functions above stay pure. */
JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrfree_MainActivity_nativeDigest(
    JNIEnv *env, jclass owner, jbyteArray input) {
  jsize n; jbyte *p=0; z_u8 out[32]; jbyteArray ret=0;
  (void)owner;
  if (!input) return 0;
  n=(*env)->GetArrayLength(env,input);
  if (n<0 || (z_u32)n>Z_MAX) return 0;
  if (n) {
    p=(*env)->GetByteArrayElements(env,input,0);
    if (!p) return 0;
  }
  if (rmr_zrf1_digest((const z_u8*)p,(z_u32)n,out)==0) {
    ret=(*env)->NewByteArray(env,32);
    if (ret) (*env)->SetByteArrayRegion(env,ret,0,32,(const jbyte*)out);
  }
  if (p) (*env)->ReleaseByteArrayElements(env,input,p,JNI_ABORT);
  return ret;
}
JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrfree_MainActivity_nativePack(
    JNIEnv *env, jclass owner, jbyteArray input) {
  jsize n; jbyte *p=0,*b=0; jbyteArray temp=0,ret=0;
  z_u32 written=0u,cap;
  (void)owner;
  if (!input) return 0;
  n=(*env)->GetArrayLength(env,input);
  if (n<0 || (z_u32)n>Z_MAX) return 0;
  cap=(z_u32)n+Z_HDR;
  temp=(*env)->NewByteArray(env,(jsize)cap);
  if (!temp) return 0;
  b=(*env)->GetByteArrayElements(env,temp,0);
  if (!b) return 0;
  if (n) {
    p=(*env)->GetByteArrayElements(env,input,0);
    if (!p) { (*env)->ReleaseByteArrayElements(env,temp,b,JNI_ABORT); return 0; }
  }
  if (rmr_zrf1_pack((const z_u8*)p,(z_u32)n,(z_u8*)b,cap,&written)==0) {
    ret=(*env)->NewByteArray(env,(jsize)written);
    if (ret) (*env)->SetByteArrayRegion(env,ret,0,(jsize)written,b);
  }
  if (p) (*env)->ReleaseByteArrayElements(env,input,p,JNI_ABORT);
  (*env)->ReleaseByteArrayElements(env,temp,b,JNI_ABORT);
  return ret;
}
JNIEXPORT jbyteArray JNICALL
Java_io_rafaelia_rmrfree_MainActivity_nativeUnpack(
    JNIEnv *env, jclass owner, jbyteArray input) {
  jsize n; jbyte *p=0,*b=0; jbyteArray ret=0;
  z_u32 raw,written=0u;
  (void)owner;
  if (!input) return 0;
  n=(*env)->GetArrayLength(env,input);
  if (n<(jsize)Z_HDR || (z_u32)n>Z_MAX+Z_HDR) return 0;
  p=(*env)->GetByteArrayElements(env,input,0);
  if (!p) return 0;
  raw=z_get32((const z_u8*)p+6);
  if (raw==0u) {
    z_u8 empty=0u;
    if (rmr_zrf1_unpack((const z_u8*)p,(z_u32)n,&empty,0u,&written)==0)
      ret=(*env)->NewByteArray(env,0);
  } else if (raw<=Z_MAX) {
    ret=(*env)->NewByteArray(env,(jsize)raw);
    if (ret) {
      b=(*env)->GetByteArrayElements(env,ret,0);
      if (b) {
        if (rmr_zrf1_unpack((const z_u8*)p,(z_u32)n,
             (z_u8*)b,raw,&written)!=0 || written!=raw) {
          (*env)->ReleaseByteArrayElements(env,ret,b,JNI_ABORT);
          b=0;ret=0;
        } else {
          (*env)->ReleaseByteArrayElements(env,ret,b,0);
        }
      } else ret=0;
    }
  }
  (*env)->ReleaseByteArrayElements(env,input,p,JNI_ABORT);
  return ret;
}
#endif
