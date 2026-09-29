/*
 * Copyright (c) 2026 Rafael Melo Reis.
 * SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
 *
 * Freestanding memory symbols for upstream C provider linking without libc.
 */
typedef __SIZE_TYPE__ rmr_pv1_ms_usize;
typedef __UINT8_TYPE__ rmr_pv1_ms_u8;

void *memcpy(void *d, const void *s, rmr_pv1_ms_usize n) {
    rmr_pv1_ms_u8 *o=(rmr_pv1_ms_u8*)d;
    const rmr_pv1_ms_u8 *i=(const rmr_pv1_ms_u8*)s;
    while(n!=0u){*o++=*i++;--n;}
    return d;
}
void *memset(void *d, int v, rmr_pv1_ms_usize n) {
    rmr_pv1_ms_u8 *o=(rmr_pv1_ms_u8*)d;
    rmr_pv1_ms_u8 b=(rmr_pv1_ms_u8)v;
    while(n!=0u){*o++=b;--n;}
    return d;
}

rmr_pv1_ms_usize strlen(const char *s) {
    const char *p=s;
    while(*p!='\0'){++p;}
    return (rmr_pv1_ms_usize)(p-s);
}

#if defined(__arm__) && !defined(__aarch64__)
void __aeabi_memcpy(void *d,const void *s,rmr_pv1_ms_usize n){(void)memcpy(d,s,n);}
void __aeabi_memcpy4(void *d,const void *s,rmr_pv1_ms_usize n){(void)memcpy(d,s,n);}
void __aeabi_memcpy8(void *d,const void *s,rmr_pv1_ms_usize n){(void)memcpy(d,s,n);}
void __aeabi_memset(void *d,rmr_pv1_ms_usize n,int v){(void)memset(d,v,n);}
void __aeabi_memclr(void *d,rmr_pv1_ms_usize n){(void)memset(d,0,n);}
#endif
