#include "pai_base.h"
#include "pai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int is_prime_i64(int64_t x) {
    if (x < 2) return 0;
    if (x % 2 == 0) return x == 2;
    for (int64_t d = 3; d*d <= x; d += 2) {
        if (x % d == 0) return 0;
    }
    return 1;
}

int64_t pai_prev_prime(int64_t n) {
    if (n <= 2) return -1;
    for (int64_t x = n - 1; x >= 2; --x) {
        if (is_prime_i64(x)) return x;
    }
    return -1;
}

int64_t pai_next_prime(int64_t n) {
    if (n < 2) n = 2;
    for (int64_t x = n + 1; ; ++x) {
        if (is_prime_i64(x)) return x;
    }
}

static void trim(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1]=='\n' || s[n-1]=='\r' || s[n-1]==' ' || s[n-1]=='\t')) s[--n]=0;
    while (*s==' ' || *s=='\t') memmove(s, s+1, strlen(s));
}

static int split_csv_i64(const char *csv, int64_t *out, int cap) {
    if (!csv || !*csv) return 0;
    char *tmp = strdup(csv);
    if (!tmp) return 0;

    int k = 0;
    char *save = NULL;
    for (char *tok = strtok_r(tmp, ",", &save); tok; tok = strtok_r(NULL, ",", &save)) {
        if (k >= cap) break;
        trim(tok);
        out[k++] = atoll(tok);
    }
    free(tmp);
    return k;
}

static int split_csv_i32(const char *csv, int *out, int cap) {
    if (!csv || !*csv) return 0;
    char *tmp = strdup(csv);
    if (!tmp) return 0;

    int k = 0;
    char *save = NULL;
    for (char *tok = strtok_r(tmp, ",", &save); tok; tok = strtok_r(NULL, ",", &save)) {
        if (k >= cap) break;
        trim(tok);
        out[k++] = atoi(tok);
    }
    free(tmp);
    return k;
}

static void to_base_vec_u64(uint64_t v, uint32_t base, uint32_t *digits, int *nd) {
    // digits em ordem reversa
    int k = 0;
    if (base < 2) { *nd = 0; return; }
    if (v == 0) { digits[0] = 0; *nd = 1; return; }
    while (v && k < 256) {
        digits[k++] = (uint32_t)(v % base);
        v /= base;
    }
    *nd = k;
}

static void fmt_base(FILE *f, int64_t value, uint32_t base) {
    if (base < 2) {
        fprintf(f, "base %u: [invalida]\n", base);
        return;
    }

    int neg = (value < 0);
    uint64_t v = (uint64_t)(neg ? -(value + 1) + 1 : value); // safe abs

    uint32_t digits[256];
    int nd = 0;
    to_base_vec_u64(v, base, digits, &nd);

    // base "bonita" <= 62 (0-9A-Za-z)
    static const char *ALPHA62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if (base <= 62) {
        char buf[512];
        int p = 0;
        if (neg) buf[p++] = '-';
        for (int i = nd - 1; i >= 0; --i) {
            uint32_t d = digits[i];
            buf[p++] = ALPHA62[d];
        }
        buf[p] = 0;
        fprintf(f, "base %u: %s\n", base, buf);
    } else {
        // bases grandes: vetor de dígitos (sem inventar alfabeto)
        fprintf(f, "base %u: %s[", base, neg ? "-" : "");
        for (int i = nd - 1; i >= 0; --i) {
            fprintf(f, "%u", digits[i]);
            if (i) fputc(',', f);
        }
        fprintf(f, "]\n");
    }
}

static void write_report(
    const char *out_dir,
    const int64_t *vals, int nvals,
    const int *bases, int nbases
) {
    pai_mkdir_p(out_dir);

    char path[PAI_MAX_PATH];
    snprintf(path, sizeof(path), "%s/bases.txt", out_dir);

    FILE *f = fopen(path, "wb");
    if (!f) pai_die("bases.txt");

    fprintf(f, "PAI :: bases report\n");
    fprintf(f, "values=%d bases=%d\n\n", nvals, nbases);

    for (int i = 0; i < nvals; ++i) {
        int64_t v = vals[i];
        int64_t pprev = pai_prev_prime(v);
        int64_t pnext = pai_next_prime(v);

        fprintf(f, "== value: %lld ==\n", (long long)v);
        fprintf(f, "prev_prime=%lld next_prime=%lld\n", (long long)pprev, (long long)pnext);

        // extras: “pontos bonitos”
        // 12^2=144, 42-30=12 etc (só calcula, não dogmatiza)
        if (v == 12) fprintf(f, "note: 12^2=144\n");
        if (v == 42) fprintf(f, "note: 42-30=12 (ciclo 4 setores)\n");
        if (v == 30) fprintf(f, "note: sin(30°)=0.5 (triangulo)\n");

        for (int b = 0; b < nbases; ++b) {
            uint32_t base = (uint32_t)bases[b];
            fmt_base(f, v, base);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("[OK] bases report: %s\n", path);
}

int pai_cmd_bases(int argc, char **argv) {
    const char *values_csv = NULL;
    const char *bases_csv  = NULL;
    const char *out_dir    = "out_bases";

    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "--values") && i + 1 < argc) values_csv = argv[++i];
        else if (!strcmp(argv[i], "--bases") && i + 1 < argc) bases_csv = argv[++i];
        else if (!strcmp(argv[i], "--out") && i + 1 < argc) out_dir = argv[++i];
    }

    if (!values_csv || !bases_csv) {
        fprintf(stderr, "uso: pai bases --values 7,12,30,... --bases 10,7,14,60,... [--out DIR]\n");
        return 1;
    }

    int64_t vals[256];
    int bases[256];
    int nvals  = split_csv_i64(values_csv, vals, 256);
    int nbases = split_csv_i32(bases_csv, bases, 256);

    if (nvals <= 0 || nbases <= 0) {
        fprintf(stderr, "[erro] values/bases vazios\n");
        return 1;
    }

    // valida base >=2
    for (int i = 0; i < nbases; ++i) {
        if (bases[i] < 2) {
            fprintf(stderr, "[erro] base invalida: %d (min=2)\n", bases[i]);
            return 1;
        }
    }

    write_report(out_dir, vals, nvals, bases, nbases);
    return 0;
}
