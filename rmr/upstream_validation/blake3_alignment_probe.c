/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Input alignment differential probe. It avoids output/input aliasing and other
 * caller behavior that would violate the public API contract.
 */
#include "blake3.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t sizes[] = {
  0u,1u,31u,32u,63u,64u,65u,255u,256u,1023u,1024u,1025u,4096u
};

static void print_hex(const uint8_t *p, size_t n) {
  size_t i;
  for (i=0; i<n; ++i) printf("%02x", p[i]);
}

int main(void) {
  const size_t max_size = 4096u;
  uint8_t *raw = (uint8_t *)malloc(max_size + 64u);
  size_t offset, s, i;
  if (raw == NULL) return 2;

  for (i=0; i<max_size+64u; ++i) raw[i]=(uint8_t)((i*131u+17u)&255u);

  for (offset=0; offset<64u; ++offset) {
    for (s=0; s<sizeof(sizes)/sizeof(sizes[0]); ++s) {
      uint8_t out[32];
      blake3_hasher h;
      blake3_hasher_init(&h);
      blake3_hasher_update(&h, raw+offset, sizes[s]);
      blake3_hasher_finalize(&h, out, sizeof(out));
      printf("offset=%zu,size=%zu,digest=", offset, sizes[s]);
      print_hex(out,sizeof(out));
      putchar('\n');
    }
  }
  free(raw);
  return 0;
}
