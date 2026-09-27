/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * RMR adapter only. BLAKE3 algorithm, API and digest semantics remain upstream.
 * File/OS I/O is isolated in hash_blake3_file.c.
 */
#include "pai_hash.h"
#include "blake3.h"

void pai_digest_hex32(const uint8_t digest[32], char out[65]) {
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0u; i < 32u; ++i) {
        out[i * 2u] = hex[digest[i] >> 4];
        out[i * 2u + 1u] = hex[digest[i] & 0x0fu];
    }
    out[64] = 0;
}

int pai_blake3_bytes(const void *data, size_t len, uint8_t out[32]) {
    if ((data == NULL && len != 0u) || out == NULL) {
        return -1;
    }

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, data, len);
    blake3_hasher_finalize(&hasher, out, 32u);
    return 0;
}
