/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Differential semantic probe. Compile the same source against official and
 * fork libraries; stdout must be byte-for-byte identical.
 */
#include "blake3.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void hex(const uint8_t *p, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) printf("%02x", p[i]);
  putchar('\n');
}

static void fill(uint8_t *p, size_t n) {
  uint32_t x = UINT32_C(0x6d2b79f5);
  size_t i;
  for (i = 0; i < n; ++i) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    p[i] = (uint8_t)x;
  }
}

static int incremental_equivalence(const uint8_t *input, size_t n) {
  static const size_t chunks[] = {1u, 7u, 64u, 255u, 1024u};
  uint8_t expected[32];
  uint8_t got[32];
  blake3_hasher one;
  size_t c;

  blake3_hasher_init(&one);
  blake3_hasher_update(&one, input, n);
  blake3_hasher_finalize(&one, expected, sizeof(expected));

  for (c = 0; c < sizeof(chunks)/sizeof(chunks[0]); ++c) {
    blake3_hasher h;
    size_t off = 0u;
    blake3_hasher_init(&h);
    while (off < n) {
      size_t take = chunks[c];
      if (take > n - off) take = n - off;
      blake3_hasher_update(&h, input + off, take);
      off += take;
    }
    blake3_hasher_finalize(&h, got, sizeof(got));
    if (memcmp(expected, got, sizeof(got)) != 0) return 0;
  }
  return 1;
}

int main(void) {
  uint8_t input[4096];
  uint8_t key[32];
  uint8_t out[128];
  uint8_t reset_a[32], reset_b[32];
  blake3_hasher h;
  size_t i;

  fill(input, sizeof(input));
  for (i = 0; i < sizeof(key); ++i) key[i] = (uint8_t)i;

  blake3_hasher_init(&h);
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, out, 32u);
  printf("hash32="); hex(out, 32u);

  blake3_hasher_init_keyed(&h, key);
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, out, 32u);
  printf("keyed32="); hex(out, 32u);

  blake3_hasher_init_derive_key(&h, "RMR upstream differential probe v3");
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, out, 32u);
  printf("derive32="); hex(out, 32u);

  blake3_hasher_init(&h);
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, out, 128u);
  printf("xof128="); hex(out, 128u);

  blake3_hasher_finalize_seek(&h, UINT64_C(1024), out, 64u);
  printf("seek1024_64="); hex(out, 64u);

  blake3_hasher_init(&h);
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, reset_a, sizeof(reset_a));
  blake3_hasher_reset(&h);
  blake3_hasher_update(&h, input, sizeof(input));
  blake3_hasher_finalize(&h, reset_b, sizeof(reset_b));
  printf("reset_equivalence=%s\n",
         memcmp(reset_a, reset_b, sizeof(reset_a)) == 0 ? "PASS" : "FAIL");

  printf("incremental_equivalence=%s\n",
         incremental_equivalence(input, sizeof(input)) ? "PASS" : "FAIL");

  return 0;
}
