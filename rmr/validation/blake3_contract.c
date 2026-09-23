/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Public C ABI compile contract. This file proves that the public header/API
 * type-checks for a target; it does not prove the implementation executes.
 */
#include "blake3.h"

void rmr_blake3_contract(const void *input, size_t input_len,
                         unsigned char out[BLAKE3_OUT_LEN]) {
  blake3_hasher hasher;
  blake3_hasher_init(&hasher);
  blake3_hasher_update(&hasher, input, input_len);
  blake3_hasher_finalize(&hasher, out, BLAKE3_OUT_LEN);
}
