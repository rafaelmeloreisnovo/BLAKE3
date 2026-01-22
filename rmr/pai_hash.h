/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t h[8];
    uint64_t len;
    uint8_t  buf[64];
    size_t   buf_len;
} pai_sha256_ctx;

void pai_sha256_init(pai_sha256_ctx *ctx);
void pai_sha256_update(pai_sha256_ctx *ctx, const uint8_t *data, size_t len);
void pai_sha256_final(pai_sha256_ctx *ctx, uint8_t out[32]);

// helpers
void pai_sha256_hex(const uint8_t hash[32], char out[65]);
int  pai_sha256_file(const char *path, uint8_t out[32]);
