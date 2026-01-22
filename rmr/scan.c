/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#define _XOPEN_SOURCE 700
#include "pai_scan.h"
#include "pai_hash.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef PAI_MAX_ENTRIES
#define PAI_MAX_ENTRIES 200000
#endif

typedef struct {
    char *name;
    int is_dir;
} entry_t;

static int cmp_entry(const void *a, const void *b) {
    const entry_t *ea = (const entry_t*)a;
    const entry_t *eb = (const entry_t*)b;
    return strcmp(ea->name, eb->name);
}

static int is_hidden_name(const char *n) {
    return (n[0]=='.' && strcmp(n,".")!=0 && strcmp(n,"..")!=0);
}

static void join_path(char *dst, size_t cap, const char *a, const char *b) {
    if(!a || !b) { snprintf(dst, cap, ""); return; }
    size_t la = strlen(a);
    if(la==0) { snprintf(dst, cap, "%s", b); return; }
    if(a[la-1]=='/') snprintf(dst, cap, "%s%s", a, b);
    else snprintf(dst, cap, "%s/%s", a, b);
}

static const char* rel_path(const char *base, const char *full) {
    size_t lb = strlen(base);
    if(lb==0) return full;
    if(strncmp(base, full, lb)==0) {
        if(full[lb]=='/') return full + lb + 1;
        if(full[lb]==0)  return ".";
    }
    return full;
}

static int path_eq_or_prefix(const char *p, const char *q) {
    // true se q == p OU q começa com p + "/"
    size_t lp = strlen(p);
    if(strcmp(p, q)==0) return 1;
    if(strncmp(p, q, lp)==0 && q[lp]=='/') return 1;
    return 0;
}

static int is_excluded(const pai_scan_opts *opt, const char *base, const char *full) {
    const char *rp = rel_path(base, full);

    for(int i=0;i<opt->nexcludes;i++) {
        const char *ex = opt->excludes[i];
        if(!ex || !*ex) continue;

        // ex pode ser relativo (ao base) ou absoluto
        if(ex[0]=='/') {
            if(path_eq_or_prefix(ex, full)) return 1;
        } else {
            if(path_eq_or_prefix(ex, rp)) return 1;
        }
    }
    return 0;
}

static void sha256_line_update(pai_sha256_ctx *mctx, const char *rel, long long size, const char hex[65]) {
    char buf[PAI_MAX_PATH + 128];
    int n = snprintf(buf, sizeof(buf), "%s\t%lld\t%s\n", rel, size, hex);
    if(n > 0) pai_sha256_update(mctx, (const uint8_t*)buf, (size_t)n);
}

static int scan_dir_rec(
    const pai_scan_opts *opt,
    const char *base,
    const char *dirpath,
    int depth,
    FILE *mf,
    pai_sha256_ctx *mctx
) {
    if(opt->max_depth >= 0 && depth > opt->max_depth) return 0;

    if(is_excluded(opt, base, dirpath)) return 0;

    DIR *d = opendir(dirpath);
    if(!d) {
        fprintf(stderr, "[scan] nao abriu dir: %s (%s)\n", dirpath, strerror(errno));
        return -1;
    }

    entry_t *entries = NULL;
    size_t n=0, cap=0;

    for(;;) {
        errno = 0;
        struct dirent *de = readdir(d);
        if(!de) break;

        const char *name = de->d_name;
        if(!strcmp(name,".") || !strcmp(name,"..")) continue;
        if(!opt->include_hidden && is_hidden_name(name)) continue;

        if(n >= PAI_MAX_ENTRIES) {
            closedir(d);
            fprintf(stderr,"[scan] limite de entradas atingido\n");
            return -1;
        }

        if(n==cap) {
            cap = cap? cap*2 : 128;
            entries = (entry_t*)realloc(entries, cap*sizeof(entry_t));
            if(!entries) { closedir(d); perror("realloc"); return -1; }
        }

        entries[n].name = strdup(name);
        if(!entries[n].name) { closedir(d); perror("strdup"); return -1; }

        char full[PAI_MAX_PATH];
        join_path(full, sizeof(full), dirpath, name);

        // excluir já na leitura
        if(is_excluded(opt, base, full)) {
            free(entries[n].name);
            continue;
        }

        struct stat st;
        int rc = opt->follow_symlinks ? stat(full, &st) : lstat(full, &st);
        if(rc != 0) entries[n].is_dir = 0;
        else entries[n].is_dir = S_ISDIR(st.st_mode) ? 1 : 0;
        n++;
    }

    closedir(d);

    if(errno != 0) {
        fprintf(stderr, "[scan] erro readdir em %s: %s\n", dirpath, strerror(errno));
    }

    qsort(entries, n, sizeof(entry_t), cmp_entry);

    for(size_t i=0;i<n;i++) {
        char full[PAI_MAX_PATH];
        join_path(full, sizeof(full), dirpath, entries[i].name);

        if(is_excluded(opt, base, full)) {
            free(entries[i].name);
            continue;
        }

        struct stat st;
        int rc = opt->follow_symlinks ? stat(full, &st) : lstat(full, &st);
        if(rc != 0) {
            free(entries[i].name);
            continue;
        }

        if(S_ISDIR(st.st_mode)) {
            int r = scan_dir_rec(opt, base, full, depth+1, mf, mctx);
            (void)r;
        } else if(S_ISREG(st.st_mode)) {
            long long sz = (long long)st.st_size;
            if(opt->max_size >= 0 && sz > opt->max_size) {
                free(entries[i].name);
                continue;
            }

            uint8_t h[32];
            char hex[65];
            if(pai_sha256_file(full, h) != 0) {
                free(entries[i].name);
                continue;
            }
            pai_sha256_hex(h, hex);

            const char *rp = rel_path(base, full);
            fprintf(mf, "%s\t%lld\t%s\n", rp, sz, hex);
            sha256_line_update(mctx, rp, sz, hex);
        }

        free(entries[i].name);
    }

    free(entries);
    return 0;
}

int pai_scan_run(const pai_scan_opts *opt) {
    if(!opt || !opt->base_dir || !opt->out_dir) return 1;

    // fail-fast: base precisa existir e abrir
    DIR *test = opendir(opt->base_dir);
    if(!test) {
        fprintf(stderr, "[scan] base invalida: %s (%s)\n", opt->base_dir, strerror(errno));
        return 3;
    }
    closedir(test);

    if (pai_mkdir_p(opt->out_dir) != 0) {
        perror("mkdir");
        return 4;
    }

    char manifest_path[PAI_MAX_PATH];
    char merkle_path[PAI_MAX_PATH];
    join_path(manifest_path, sizeof(manifest_path), opt->out_dir, "manifest.tsv");
    join_path(merkle_path, sizeof(merkle_path), opt->out_dir, "merkle_root.txt");

    FILE *mf = fopen(manifest_path, "wb");
    if(!mf) { perror("manifest"); return 1; }

    pai_sha256_ctx mctx;
    pai_sha256_init(&mctx);

    int rc = scan_dir_rec(opt, opt->base_dir, opt->base_dir, 0, mf, &mctx);
    fclose(mf);

    uint8_t root[32];
    char hex[65];
    pai_sha256_final(&mctx, root);
    pai_sha256_hex(root, hex);

    FILE *rf = fopen(merkle_path, "wb");
    if(!rf) { perror("merkle_root"); return 1; }
    fprintf(rf, "%s\n", hex);
    fclose(rf);

    printf("[OK] manifest: %s\n", manifest_path);
    printf("[OK] merkle_root: %s\n", merkle_path);
    printf("[OK] root=%s\n", hex);

    return (rc==0)? 0 : 2;
}

int pai_cmd_scan(int argc, char **argv) {
    pai_scan_opts opt;
    memset(&opt, 0, sizeof(opt));
    opt.include_hidden = 0;
    opt.follow_symlinks = 0;
    opt.max_depth = -1;
    opt.max_size = -1;

    const char *ex_list[64];
    int ex_n = 0;

    for(int i=2;i<argc;i++) {
        if(!strcmp(argv[i],"--base") && i+1<argc) opt.base_dir = argv[++i];
        else if(!strcmp(argv[i],"--out") && i+1<argc) opt.out_dir = argv[++i];
        else if(!strcmp(argv[i],"--hidden")) opt.include_hidden = 1;
        else if(!strcmp(argv[i],"--follow")) opt.follow_symlinks = 1;
        else if(!strcmp(argv[i],"--max-depth") && i+1<argc) opt.max_depth = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--max-size") && i+1<argc) opt.max_size = atoll(argv[++i]);
        else if(!strcmp(argv[i],"--exclude") && i+1<argc) {
            if(ex_n < 64) ex_list[ex_n++] = argv[++i];
            else i++;
        }
    }

    if(!opt.base_dir || !opt.out_dir) {
        fprintf(stderr, "uso: pai scan --base DIR --out OUTDIR [--exclude PATH]... [--hidden] [--follow] [--max-depth N] [--max-size BYTES]\n");
        return 1;
    }

    // por padrão: exclui o próprio out_dir (se estiver dentro do base)
    ex_list[ex_n++] = opt.out_dir;

    opt.excludes = ex_list;
    opt.nexcludes = ex_n;

    return pai_scan_run(&opt);
}
