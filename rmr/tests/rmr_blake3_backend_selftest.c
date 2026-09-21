/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "pai_hash.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    static const char expected[] =
        "6437b3ac38465133ffb63b75273a8db548c558465d79db03fd359c6cd5bd9d85";
    const unsigned char input[] = {'a', 'b', 'c'};
    unsigned char digest[32];
    char hex[65];

    if(pai_blake3_bytes(input, sizeof(input), digest) != 0) {
        fputs("RMR_BLAKE3_BACKEND_FAIL: adapter error\n", stderr);
        return 1;
    }

    pai_digest_hex32(digest, hex);
    if(strcmp(hex, expected) != 0) {
        fprintf(stderr, "RMR_BLAKE3_BACKEND_FAIL: got=%s expected=%s\n", hex, expected);
        return 2;
    }

    puts("RMR_BLAKE3_BACKEND_OK");
    return 0;
}
