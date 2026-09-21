/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * RMR adapter only. BLAKE3 algorithm, API and digest semantics remain upstream.
 */

#include "pai_hash.h"
#include "blake3.h"

#include <stdio.h>

void pai_digest_hex32(const uint8_t digest[32], char out[65]) {
    static const char hex[] = "0123456789abcdef";
    for(size_t i = 0; i < 32; ++i) {
        out[i * 2] = hex[digest[i] >> 4];
        out[i * 2 + 1] = hex[digest[i] & 0x0f];
    }
    out[64] = 0;
}

int pai_blake3_bytes(const void *data, size_t len, uint8_t out[32]) {
    if((data == NULL && len != 0) || out == NULL) return -1;

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, data, len);
    blake3_hasher_finalize(&hasher, out, 32);
    return 0;
}

int pai_blake3_file(const char *path, uint8_t out[32]) {
    if(path == NULL || out == NULL) return -1;

    FILE *f = fopen(path, "rb");
    if(!f) return -1;

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    uint8_t buf[64 * 1024];
    for(;;) {
        size_t n = fread(buf, 1, sizeof(buf), f);
        if(n > 0) blake3_hasher_update(&hasher, buf, n);
        if(n < sizeof(buf)) {
            if(ferror(f)) {
                fclose(f);
                return -1;
            }
            break;
        }
    }

    fclose(f);
    blake3_hasher_finalize(&hasher, out, 32);
    return 0;
}
