/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai_sign.h"
#include "pai_hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

static int deterministic_enabled(void) {
    const char *env = getenv("RMR_DETERMINISTIC");
    return env && strcmp(env, "1") == 0;
}

static void signature_timestamp(struct tm *out_tm) {
    if(deterministic_enabled()) {
        memset(out_tm, 0, sizeof(*out_tm));
        out_tm->tm_year = 70;
        out_tm->tm_mon = 0;
        out_tm->tm_mday = 1;
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = gmtime(&t);
    if(tm) {
        *out_tm = *tm;
        return;
    }
    memset(out_tm, 0, sizeof(*out_tm));
    out_tm->tm_year = 70;
    out_tm->tm_mon = 0;
    out_tm->tm_mday = 1;
}

static void usage(void) {
    puts("uso:");
    puts("  pai sign --base DIR --scan SCANDIR --out DIR");
}

/* le arquivo texto pequeno (ex: merkle_root.txt) */
static int read_text(const char *path, char *buf, size_t max) {
    FILE *f = fopen(path, "r");
    if(!f) return -1;
    size_t n = fread(buf, 1, max-1, f);
    buf[n] = 0;
    fclose(f);
    return 0;
}

static void hex32(const uint8_t in[32], char out_hex[65]) {
    static const char *hx = "0123456789abcdef";
    for(int i=0;i<32;i++) {
        out_hex[i*2+0] = hx[(in[i] >> 4) & 0xF];
        out_hex[i*2+1] = hx[(in[i] >> 0) & 0xF];
    }
    out_hex[64] = 0;
}

int pai_cmd_sign(int argc, char **argv) {
    const char *base = NULL;
    const char *scan = NULL;
    const char *out  = NULL;

    for(int i=2;i<argc;i++) {
        if(!strcmp(argv[i], "--base") && i+1<argc) base = argv[++i];
        else if(!strcmp(argv[i], "--scan") && i+1<argc) scan = argv[++i];
        else if(!strcmp(argv[i], "--out") && i+1<argc) out = argv[++i];
    }

    if(!base || !scan || !out) {
        usage();
        return 1;
    }

    (void)base; /* reservado p/ v2: incluir hash do base/manifest no signature */

    /* ler merkle_root do scan */
    char merkle[256];
    char path_merkle[512];
    snprintf(path_merkle, sizeof(path_merkle), "%s/merkle_root.txt", scan);

    if(read_text(path_merkle, merkle, sizeof(merkle)) != 0) {
        fprintf(stderr, "[sign] falhou lendo: %s\n", path_merkle);
        return 2;
    }

    /* hash do binario ./pai usando API existente */
    uint8_t hbin[32];
    if(pai_sha256_file("./pai", hbin) != 0) {
        fprintf(stderr, "[sign] falhou hash binario: ./pai\n");
        return 3;
    }

    char self_hex[65];
    hex32(hbin, self_hex);

    struct tm tm;
    signature_timestamp(&tm);

    char sigpath[512];
    snprintf(sigpath, sizeof(sigpath), "%s/SIGNATURE.txt", out);

    FILE *f = fopen(sigpath, "w");
    if(!f) {
        perror("fopen");
        return 4;
    }

    fprintf(f, "PAI SIGNATURE v1\n");
    fprintf(f, "timestamp=%04d-%02d-%02dT%02d:%02d:%02dZ\n",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);

    fprintf(f, "binary_sha256=%s\n", self_hex);
    fprintf(f, "merkle_root=%s", merkle);

    fclose(f);

    printf("[OK] assinatura gerada: %s\n", sigpath);
    return 0;
}
