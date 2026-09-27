/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Hosted file adapter for the RMR SHA-256 in-memory core.
 * This translation unit is intentionally NOT freestanding.
 */
#include "pai_hash.h"

#include <stdio.h>

int pai_sha256_file(const char *path, uint8_t out[32]) {
    if (path == NULL || out == NULL) {
        return -1;
    }

    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return -1;
    }

    pai_sha256_ctx ctx;
    pai_sha256_init(&ctx);

    uint8_t buf[4096];
    for (;;) {
        const size_t n = fread(buf, 1u, sizeof(buf), f);
        if (n != 0u) {
            pai_sha256_update(&ctx, buf, n);
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
    pai_sha256_final(&ctx, out);
    return 0;
}
