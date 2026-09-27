/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under rmr/LICENSE_RMR.
 *
 * Fixed-shape secret zeroization.
 * No libc, heap, OS, provider, if/for/while or variable-length loop.
 */
#include "../include/rmr_cf140_secret_buffer.h"

#define RMR_CF_ZERO8(p,o)     (p)->bytes[(o)+0u]=0u; (p)->bytes[(o)+1u]=0u;     (p)->bytes[(o)+2u]=0u; (p)->bytes[(o)+3u]=0u;     (p)->bytes[(o)+4u]=0u; (p)->bytes[(o)+5u]=0u;     (p)->bytes[(o)+6u]=0u; (p)->bytes[(o)+7u]=0u

void rmr_cf_secret_line_zero(rmr_cf_secret_line *line) {
    RMR_CF_ZERO8(line, 0u);
    RMR_CF_ZERO8(line, 8u);
    RMR_CF_ZERO8(line, 16u);
    RMR_CF_ZERO8(line, 24u);
    RMR_CF_ZERO8(line, 32u);
    RMR_CF_ZERO8(line, 40u);
    RMR_CF_ZERO8(line, 48u);
    RMR_CF_ZERO8(line, 56u);
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("" ::: "memory");
#endif
}

void rmr_cf_secret_workspace_zero(rmr_cf_secret_workspace *workspace) {
    rmr_cf_secret_line_zero(&workspace->key);
    rmr_cf_secret_line_zero(&workspace->work0);
    rmr_cf_secret_line_zero(&workspace->work1);
}

#undef RMR_CF_ZERO8
