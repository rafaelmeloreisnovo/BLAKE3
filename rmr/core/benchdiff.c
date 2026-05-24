/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai.h"
#include "pai_benchdiff.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PAI_BENCHDIFF_MAX_SAMPLES 131072

typedef struct {
    double avg;
    double median;
    double p95;
    double variance;
    int n_valid;
    int ok_count;
    int fail_count;
} bench_stats;

static int cmp_double_asc(const void *a, const void *b) {
    const double da = *(const double*)a;
    const double db = *(const double*)b;
    return (da > db) - (da < db);
}

static int ensure_dir(const char *path) {
    if(!path || !*path) return 0;
    if(mkdir(path, 0700) == 0) return 0;
    if(errno == EEXIST) return 0;
    return -1;
}

static int read_tsv_stats(const char *tsv, bench_stats *out) {
    FILE *f = fopen(tsv, "r");
    if(!f) return -1;

    char line[512];
    double values[PAI_BENCHDIFF_MAX_SAMPLES];
    int n = 0;
    int ok_count = 0;
    int fail_count = 0;

    if(!fgets(line, sizeof(line), f)) {
        fclose(f);
        return -2;
    }

    while(fgets(line, sizeof(line), f)) {
        char cmd[256];
        int ok = 0;
        double ms = 0.0;

        if(sscanf(line, "%255s %d %lf", cmd, &ok, &ms) != 3) continue;
        if(ok) ok_count++; else fail_count++;
        if(ok != 1) continue;
        if(ms < 0.0) continue;
        if(n >= PAI_BENCHDIFF_MAX_SAMPLES) continue;
        values[n++] = ms;
    }
    fclose(f);

    if(n <= 0) return -3;

    double sum = 0.0;
    for(int i=0;i<n;i++) sum += values[i];
    double avg = sum / (double)n;

    double var = 0.0;
    for(int i=0;i<n;i++) {
        const double d = values[i] - avg;
        var += d * d;
    }
    var /= (double)n;

    qsort(values, (size_t)n, sizeof(values[0]), cmp_double_asc);
    double median = (n % 2) ? values[n/2] : (values[(n/2)-1] + values[n/2]) * 0.5;

    int p95i = (int)ceil(0.95 * (double)n) - 1;
    if(p95i < 0) p95i = 0;
    if(p95i >= n) p95i = n - 1;

    out->avg = avg;
    out->median = median;
    out->p95 = values[p95i];
    out->variance = var;
    out->n_valid = n;
    out->ok_count = ok_count;
    out->fail_count = fail_count;
    return 0;
}

int pai_cmd_benchdiff(int argc, char **argv) {
    const char *a = NULL;
    const char *b = NULL;
    const char *out = NULL;
    double threshold = 5.0;

    for(int i = 2; i < argc; i++) {
        if(!strcmp(argv[i], "--a") && i+1 < argc) { a = argv[++i]; continue; }
        if(!strcmp(argv[i], "--b") && i+1 < argc) { b = argv[++i]; continue; }
        if(!strcmp(argv[i], "--out") && i+1 < argc) { out = argv[++i]; continue; }
        if(!strcmp(argv[i], "--threshold") && i+1 < argc) { threshold = atof(argv[++i]); continue; }
    }

    if(!a || !b) {
        fprintf(stderr, "uso: pai benchdiff --a A.tsv --b B.tsv [--out OUTDIR] [--threshold 5]\n");
        return 2;
    }

    bench_stats sa, sb;
    if(read_tsv_stats(a, &sa) != 0) {
        fprintf(stderr, "[erro] nao leu A: %s\n", a);
        return 3;
    }
    if(read_tsv_stats(b, &sb) != 0) {
        fprintf(stderr, "[erro] nao leu B: %s\n", b);
        return 4;
    }

    const double delta_avg = ((sb.avg - sa.avg) / sa.avg) * 100.0;
    const double delta_p95 = ((sb.p95 - sa.p95) / sa.p95) * 100.0;
    const double abs_avg = fabs(delta_avg);
    const double abs_p95 = fabs(delta_p95);
    const char *status = (abs_avg >= threshold || abs_p95 >= threshold) ? "ALERT" : "OK";

    if(out && *out) {
        if(ensure_dir(out) != 0) {
            perror("mkdir");
            return 5;
        }

        char rpt[512];
        char json[512];
        snprintf(rpt, sizeof(rpt), "%s/benchdiff_report.txt", out);
        snprintf(json, sizeof(json), "%s/benchdiff_report.json", out);

        FILE *o = fopen(rpt, "w");
        if(!o) {
            perror("fopen");
            return 6;
        }

        fprintf(o, "BENCHDIFF\n");
        fprintf(o, "A=%s\n", a);
        fprintf(o, "B=%s\n", b);
        fprintf(o, "A.avg_ms=%.3f (n=%d)\n", sa.avg, sa.n_valid);
        fprintf(o, "B.avg_ms=%.3f (n=%d)\n", sb.avg, sb.n_valid);
        fprintf(o, "A.median_ms=%.3f\n", sa.median);
        fprintf(o, "B.median_ms=%.3f\n", sb.median);
        fprintf(o, "A.p95_ms=%.3f\n", sa.p95);
        fprintf(o, "B.p95_ms=%.3f\n", sb.p95);
        fprintf(o, "A.variance=%.6f\n", sa.variance);
        fprintf(o, "B.variance=%.6f\n", sb.variance);
        fprintf(o, "A.ok=%d fail=%d\n", sa.ok_count, sa.fail_count);
        fprintf(o, "B.ok=%d fail=%d\n", sb.ok_count, sb.fail_count);
        fprintf(o, "delta.avg=%.2f%%\n", delta_avg);
        fprintf(o, "delta.p95=%.2f%%\n", delta_p95);
        fprintf(o, "threshold=%.2f%%\n", threshold);
        fprintf(o, "status=%s\n", status);
        fclose(o);

        FILE *j = fopen(json, "w");
        if(j) {
            fprintf(j,
                "{\"a\":{\"path\":\"%s\",\"avg_ms\":%.6f,\"median_ms\":%.6f,\"p95_ms\":%.6f,\"variance\":%.9f,\"valid_samples\":%d,\"ok_count\":%d,\"fail_count\":%d},"
                "\"b\":{\"path\":\"%s\",\"avg_ms\":%.6f,\"median_ms\":%.6f,\"p95_ms\":%.6f,\"variance\":%.9f,\"valid_samples\":%d,\"ok_count\":%d,\"fail_count\":%d},"
                "\"delta_avg_pct\":%.6f,\"delta_p95_pct\":%.6f,\"threshold_pct\":%.6f,\"status\":\"%s\"}\n",
                a, sa.avg, sa.median, sa.p95, sa.variance, sa.n_valid, sa.ok_count, sa.fail_count,
                b, sb.avg, sb.median, sb.p95, sb.variance, sb.n_valid, sb.ok_count, sb.fail_count,
                delta_avg, delta_p95, threshold, status);
            fclose(j);
        }

        printf("[OK] benchdiff_report.txt: %s\n", rpt);
        printf("[OK] benchdiff_report.json: %s\n", json);
    } else {
        printf("BENCHDIFF\n");
        printf("A.avg_ms=%.3f (n=%d)\n", sa.avg, sa.n_valid);
        printf("B.avg_ms=%.3f (n=%d)\n", sb.avg, sb.n_valid);
        printf("A.p95_ms=%.3f\n", sa.p95);
        printf("B.p95_ms=%.3f\n", sb.p95);
        printf("delta.avg=%.2f%%\n", delta_avg);
        printf("delta.p95=%.2f%%\n", delta_p95);
        printf("threshold=%.2f%%\n", threshold);
        printf("status=%s\n", status);
    }

    return (strcmp(status, "ALERT") == 0) ? 1 : 0;
}
