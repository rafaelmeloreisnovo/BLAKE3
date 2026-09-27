/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Hosted file adapter for the upstream BLAKE3 byte API.
 * This translation unit is intentionally NOT freestanding.
 */
#include "pai_hash.h"
#include "blake3.h"

#include <stdio.h>

int pai_blake3_file(const char *path, uint8_t out[32]) {
    if (path == NULL || out == NULL) {
        return -1;
    }

    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    uint8_t buf[64u * 1024u];
    for (;;) {
        const size_t n = fread(buf, 1u, sizeof(buf), f);
        if (n != 0u) {
            blake3_hasher_update(&hasher, buf, n);
        }
        if (n < sizeof(buf)) {
            if (ferror(f) != 0) {
                fclose(f);
                return -1;
            }
            break;
        }
    }

    fclose(f);
    blake3_hasher_finalize(&hasher, out, 32u);
    return 0;
}
