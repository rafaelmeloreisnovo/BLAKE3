/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai_validate.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float alpha;
    int attractors;
    int rows;
    int cols;
    int dr;
    int dc;
    int bytes_unique;
    int transitions;
    int length;
} pai_validate_cfg;

static int gcd_i(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a < 0 ? -a : a;
}

static void validate_usage(void) {
    puts("uso: pai validate [--alpha 0.25] [--attractors 42] [--rows 10] [--cols 10] [--dr 7] [--dc 9] [--unique U] [--transitions T] [--len N]");
}

int pai_cmd_validate(int argc, char **argv) {
    pai_validate_cfg cfg;
    cfg.alpha = 0.25f;
    cfg.attractors = 42;
    cfg.rows = 10;
    cfg.cols = 10;
    cfg.dr = 7;
    cfg.dc = 9;
    cfg.bytes_unique = 0;
    cfg.transitions = 0;
    cfg.length = 1;

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--alpha") && i + 1 < argc) cfg.alpha = (float)atof(argv[++i]);
        else if (!strcmp(argv[i], "--attractors") && i + 1 < argc) cfg.attractors = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--rows") && i + 1 < argc) cfg.rows = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--cols") && i + 1 < argc) cfg.cols = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--dr") && i + 1 < argc) cfg.dr = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--dc") && i + 1 < argc) cfg.dc = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--unique") && i + 1 < argc) cfg.bytes_unique = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--transitions") && i + 1 < argc) cfg.transitions = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--len") && i + 1 < argc) cfg.length = atoi(argv[++i]);
        else {
            validate_usage();
            return 1;
        }
    }

    if (cfg.rows <= 0 || cfg.cols <= 0 || cfg.length <= 1) {
        fprintf(stderr, "[erro] rows/cols devem ser >0 e len >1\n");
        return 2;
    }

    const float alpha_expected = 0.25f;
    const int attractors_expected = 42;
    const int capacity = cfg.rows * cfg.cols;
    const double bits_geom = log2((double)capacity);
    const int gcd_r = gcd_i(cfg.dr, cfg.rows);
    const int gcd_c = gcd_i(cfg.dc, cfg.cols);
    const double entropy_milli =
        ((double)cfg.bytes_unique * 6000.0 / 256.0) +
        ((double)cfg.transitions * 2000.0 / (double)(cfg.length - 1));

    int ok = 1;
    if (fabsf(cfg.alpha - alpha_expected) > 1e-6f) ok = 0;
    if (cfg.attractors != attractors_expected) ok = 0;
    if (gcd_r != 1 || gcd_c != 1) ok = 0;

    printf("[rmr.validate] alpha=%.6f expected=%.2f\n", cfg.alpha, alpha_expected);
    printf("[rmr.validate] attractors=%d expected=%d\n", cfg.attractors, attractors_expected);
    printf("[rmr.validate] capacity=%d bits_geom=%.6f\n", capacity, bits_geom);
    printf("[rmr.validate] gcd(dr,rows)=%d gcd(dc,cols)=%d\n", gcd_r, gcd_c);
    printf("[rmr.validate] entropy_milli=%.3f\n", entropy_milli);

    if (!ok) {
        fprintf(stderr, "[rmr.validate] FAIL: invariantes violados\n");
        return 3;
    }

    puts("[rmr.validate] OK: invariantes aceitos");
    return 0;
}
