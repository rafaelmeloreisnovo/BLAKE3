/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai_benchdiff.h"
#include "pai_bench.h"
#include "pai_sign.h"
#include "pai_sign.h"
#include "pai_toroid.h"
#include "pai.h"
#include "pai_hash.h"
#include "pai_scan.h"
#include "pai_base.h"
#include "pai_geom.h"
#include <stdio.h>
#include <string.h>

static void usage(void) {
    puts("pai (Pipeline de Arquitetura Integradora) — C bare metal");
    puts("");
    puts("Comandos:");
    puts("  pai hash  --file arquivo");
    puts("  pai scan  --base DIR --out OUTDIR [--exclude PATH]... [--hidden] [--follow] [--max-depth N] [--max-size BYTES]");
    puts("  pai bases --values 7,12,30,42,56,70,144 --bases 10,7,14,60,20,18,13 --out out_bases");
    puts("  pai geom  --out out_geom --size 1024 --cycle42 --sin30 --sqrt2 --sqrt3 --fibo --shapes --mandel");
    puts("  pai toroid --tex out_geom/geom.pgm --out out_toroid [--nu 256] [--nv 128] [--R 1.0] [--r 0.35]");
    puts("  pai bench --repeat N --out OUTDIR -- <comando> [args...]");
    puts("  pai benchdiff --a A.tsv --b B.tsv [--out OUTDIR] [--threshold 5]");
    puts("  pai sign   --base DIR --scan SCANDIR --out DIR");
    puts("");
    puts("Notas:");
    puts("  - scan exclui automaticamente o OUTDIR para evitar auto-referencia.");
    puts("  - scan falha (exit!=0) se a base nao existir.");
    puts("  - geom escreve PGM (P5) para compatibilidade bare metal.");
    puts("");
}

static int cmd_hash(int argc, char **argv) {
    const char *file = NULL;
    for(int i=2;i<argc;i++) {
        if(!strcmp(argv[i],"--file") && i+1<argc) file = argv[++i];
    }
    if(!file) {
        fprintf(stderr,"uso: pai hash --file arquivo\n");
        return 1;
    }

    uint8_t hash[32];
    char hex[65];

    if(pai_sha256_file(file, hash)!=0) {
        perror("hash");
        return 1;
    }

    pai_sha256_hex(hash, hex);
    printf("%s  %s\n", hex, file);
    return 0;
}

int pai_main(int argc, char **argv) {
    if(argc < 2) { usage(); return 0; }

    if(!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
        usage();
        return 0;
    }

    if(!strcmp(argv[1], "hash"))  return cmd_hash(argc, argv);
    if(!strcmp(argv[1], "scan"))  return pai_cmd_scan(argc, argv);
    if(!strcmp(argv[1], "bases")) return pai_cmd_bases(argc, argv);
    if(!strcmp(argv[1], "geom"))  return pai_cmd_geom(argc, argv);
    if(!strcmp(argv[1], "toroid")) return pai_cmd_toroid(argc, argv);
    if(!strcmp(argv[1], "sign"))   return pai_cmd_sign(argc, argv);
    if(!strcmp(argv[1], "bench"))  return pai_cmd_bench(argc, argv);
    if(!strcmp(argv[1], "benchdiff")) return pai_cmd_benchdiff(argc, argv);
    if(!strcmp(argv[1], "benchdiff")) return pai_cmd_benchdiff(argc, argv);

    fprintf(stderr,"[erro] comando desconhecido: %s\n", argv[1]);
    usage();
    return 1;
}
