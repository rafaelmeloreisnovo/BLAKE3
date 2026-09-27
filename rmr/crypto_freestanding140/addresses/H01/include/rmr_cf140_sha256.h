/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Authorial RMR implementation expression of the SHA-256 compression core.
 * SHA-256 itself is a standard primitive, not claimed as an RMR invention.
 */
#ifndef RMR_CF140_SHA256_H
#define RMR_CF140_SHA256_H

#include "../../../include/rmr_cf140.h"

typedef struct rmr_cf_sha256_state {
    rmr_cf_u32 h0; rmr_cf_u32 h1; rmr_cf_u32 h2; rmr_cf_u32 h3;
    rmr_cf_u32 h4; rmr_cf_u32 h5; rmr_cf_u32 h6; rmr_cf_u32 h7;
} rmr_cf_sha256_state;

void rmr_cf_sha256_init(rmr_cf_sha256_state *state);
void rmr_cf_sha256_compress_block(rmr_cf_sha256_state *state, const rmr_cf_u8 block[64]);

#endif
