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

#ifndef PAI_MAX_PATH
#define PAI_MAX_PATH 4096
#endif

// --- core config ---
typedef struct {
    const char *base_path;
    const char *out_dir;

    // scan/filter
    int include_hidden;
    int follow_symlinks;
    int max_depth;      // -1 unlimited
    int64_t max_size;   // -1 unlimited
} pai_cfg_t;

// --- tiny helpers ---
void pai_die(const char *msg);
int pai_mkdir_p(const char *path);

// --- cli ---
int pai_main(int argc, char **argv);
