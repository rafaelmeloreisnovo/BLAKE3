#include "pai.h"
#include "pai_benchdiff.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int ensure_dir(const char *path) {
    if(!path || !*path) return 0;
    if(mkdir(path, 0700) == 0) return 0;
    if(errno == EEXIST) return 0;
    return -1;
}

static int read_avg_ms(const char *tsv, double *out_avg, int *out_n) {
    FILE *f = fopen(tsv, "r");
    if(!f) return -1;

    char line[256];
    int n = 0;
    double sum = 0.0;

    /* header */
    if(!fgets(line, sizeof(line), f)) {
        fclose(f);
        return -2;
    }

    while(fgets(line, sizeof(line), f)) {
        /* format: cmd \t ok \t ms */
        char cmd[64];
        int ok = 0;
        double ms = 0.0;

        /* be permissive: spaces or tabs */
        if(sscanf(line, "%63s %d %lf", cmd, &ok, &ms) != 3) continue;
        if(ok != 1) continue;
        sum += ms;
        n++;
    }

    fclose(f);

    if(n <= 0) return -3;
    *out_avg = sum / (double)n;
    *out_n = n;
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

    double a_avg = 0.0, b_avg = 0.0;
    int a_n = 0, b_n = 0;

    if(read_avg_ms(a, &a_avg, &a_n) != 0) {
        fprintf(stderr, "[erro] nao leu A: %s\n", a);
        return 3;
    }
    if(read_avg_ms(b, &b_avg, &b_n) != 0) {
        fprintf(stderr, "[erro] nao leu B: %s\n", b);
        return 4;
    }

    /* delta percent: positive means B slower than A (bigger ms) */
    double delta = ((b_avg - a_avg) / a_avg) * 100.0;
    double adelta = (delta < 0.0) ? -delta : delta;
    const char *status = (adelta >= threshold) ? "ALERT" : "OK";

    if(out && *out) {
        if(ensure_dir(out) != 0) {
            perror("mkdir");
            return 5;
        }

        char rpt[512];
        snprintf(rpt, sizeof(rpt), "%s/benchdiff_report.txt", out);

        FILE *o = fopen(rpt, "w");
        if(!o) {
            perror("fopen");
            return 6;
        }

        fprintf(o, "BENCHDIFF\n");
        fprintf(o, "A=%s\n", a);
        fprintf(o, "B=%s\n", b);
        fprintf(o, "A.avg_ms=%.3f (n=%d)\n", a_avg, a_n);
        fprintf(o, "B.avg_ms=%.3f (n=%d)\n", b_avg, b_n);
        fprintf(o, "delta=%.2f%% (threshold=%.2f%%)\n", delta, threshold);
        fprintf(o, "status=%s\n", status);
        fclose(o);

        printf("[OK] benchdiff_report.txt: %s\n", rpt);
    } else {
        printf("BENCHDIFF\n");
        printf("A.avg_ms=%.3f (n=%d)\n", a_avg, a_n);
        printf("B.avg_ms=%.3f (n=%d)\n", b_avg, b_n);
        printf("delta=%.2f%% (threshold=%.2f%%)\n", delta, threshold);
        printf("status=%s\n", status);
    }

    return (strcmp(status, "ALERT") == 0) ? 1 : 0;
}
