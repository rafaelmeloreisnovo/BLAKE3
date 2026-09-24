/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "blake3.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double seconds(const struct timespec *a, const struct timespec *b) {
  return (double)(b->tv_sec-a->tv_sec) +
         (double)(b->tv_nsec-a->tv_nsec)/1000000000.0;
}

int main(int argc, char **argv) {
#ifndef BLAKE3_USE_TBB
  (void)argc; (void)argv;
  puts("TBB=TOKEN_VAZIO_NOT_COMPILED");
  return 3;
#else
  size_t size;
  unsigned long iterations, i;
  uint8_t *buf;
  uint8_t out[32];
  struct timespec a,b;
  double dt;

  if (argc != 3) return 2;
  size=(size_t)strtoull(argv[1],NULL,10);
  iterations=strtoul(argv[2],NULL,10);
  if (!size || !iterations) return 2;
  buf=(uint8_t*)malloc(size);
  if (!buf) return 4;
  for(i=0;i<size;++i) buf[i]=(uint8_t)(i*17u+3u);

  clock_gettime(CLOCK_MONOTONIC,&a);
  for(i=0;i<iterations;++i) {
    blake3_hasher h;
    blake3_hasher_init(&h);
    blake3_hasher_update_tbb(&h,buf,size);
    blake3_hasher_finalize(&h,out,sizeof(out));
  }
  clock_gettime(CLOCK_MONOTONIC,&b);
  dt=seconds(&a,&b);
  printf("TBB_RESULT,size=%zu,iterations=%lu,seconds=%.9f,mib_s=%.3f,digest=",
         size,iterations,dt,
         ((double)size*(double)iterations/(1024.0*1024.0))/dt);
  for(i=0;i<sizeof(out);++i) printf("%02x",out[i]);
  putchar('\n');
  free(buf);
  return 0;
#endif
}
