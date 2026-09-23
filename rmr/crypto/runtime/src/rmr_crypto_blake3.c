/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Narrow adapter only. BLAKE3 algorithm and digest semantics remain upstream.
 */
#include "rmr_crypto_runtime.h"
#include "blake3.h"

int rmr_crypto_blake3_digest(const uint8_t *input, size_t input_len,
                             uint8_t *out, size_t out_cap, size_t *out_len);

int rmr_crypto_blake3_digest(const uint8_t *input, size_t input_len,
                             uint8_t *out, size_t out_cap, size_t *out_len) {
  blake3_hasher hasher;

  if (out == NULL || out_len == NULL || out_cap < BLAKE3_OUT_LEN) {
    return -1;
  }
  if (input == NULL && input_len != 0u) {
    return -1;
  }

  blake3_hasher_init(&hasher);
  if (input_len != 0u) {
    blake3_hasher_update(&hasher, input, input_len);
  }
  blake3_hasher_finalize(&hasher, out, BLAKE3_OUT_LEN);
  *out_len = BLAKE3_OUT_LEN;
  return 0;
}
