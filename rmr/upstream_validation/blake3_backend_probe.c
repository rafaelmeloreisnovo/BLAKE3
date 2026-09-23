/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
#include "blake3.h"
#include "blake3_impl.h"

#include <stdio.h>

int main(void) {
  static const unsigned char abc[] = {'a','b','c'};
  unsigned char out[BLAKE3_OUT_LEN];
  blake3_hasher h;
  size_t i;

  blake3_hasher_init(&h);
  blake3_hasher_update(&h, abc, sizeof(abc));
  blake3_hasher_finalize(&h, out, sizeof(out));

  printf("simd_degree=%zu\n", blake3_simd_degree());
  printf("abc=");
  for (i=0; i<sizeof(out); ++i) printf("%02x", out[i]);
  putchar('\n');
  return 0;
}
