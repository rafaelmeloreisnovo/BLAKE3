/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * CF140 secret-buffer substrate V1.
 *
 * Key bytes are bit-exact bytes. Q16/Q32/Q42 telemetry is intentionally
 * outside this structure.
 */
#ifndef RMR_CF140_SECRET_BUFFER_H
#define RMR_CF140_SECRET_BUFFER_H

#include "rmr_cf140.h"

#if defined(__GNUC__) || defined(__clang__)
#define RMR_CF_SECRET_ALIGN __attribute__((aligned(64)))
#else
#define RMR_CF_SECRET_ALIGN
#endif

typedef struct RMR_CF_SECRET_ALIGN rmr_cf_secret_line {
    volatile rmr_cf_u8 bytes[64];
} rmr_cf_secret_line;

typedef struct RMR_CF_SECRET_ALIGN rmr_cf_secret_workspace {
    rmr_cf_secret_line key;
    rmr_cf_secret_line work0;
    rmr_cf_secret_line work1;
} rmr_cf_secret_workspace;

_Static_assert(sizeof(rmr_cf_secret_line) == 64u,
               "secret line must occupy exactly 64 bytes");

void rmr_cf_secret_line_zero(rmr_cf_secret_line *line);
void rmr_cf_secret_workspace_zero(rmr_cf_secret_workspace *workspace);

#endif
