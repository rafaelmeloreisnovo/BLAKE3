/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/*
 * In-memory SHA-256 state/API.
 *
 * The core implementation in hash_sha256.c performs no file I/O.
 * File access is an explicit hosted adapter in hash_sha256_file.c.
 */
typedef struct {
    uint32_t h[8];
    uint64_t len;
    uint8_t  buf[64];
    size_t   buf_len;
} pai_sha256_ctx;

void pai_sha256_init(pai_sha256_ctx *ctx);
void pai_sha256_update(pai_sha256_ctx *ctx, const uint8_t *data, size_t len);
void pai_sha256_final(pai_sha256_ctx *ctx, uint8_t out[32]);

/* Pure formatting over caller-owned output buffers. */
void pai_digest_hex32(const uint8_t digest[32], char out[65]);
void pai_sha256_hex(const uint8_t hash[32], char out[65]);

/* HOSTED ADAPTER: depends on stdio/FILE in hash_sha256_file.c. */
int pai_sha256_file(const char *path, uint8_t out[32]);

/*
 * BLAKE3 byte adapter. The primitive remains the repository upstream c/
 * implementation. This function does not perform file I/O.
 */
int pai_blake3_bytes(const void *data, size_t len, uint8_t out[32]);

/* HOSTED ADAPTER: depends on stdio/FILE in hash_blake3_file.c. */
int pai_blake3_file(const char *path, uint8_t out[32]);
