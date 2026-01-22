/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

static double now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec*1000.0 + (double)ts.tv_nsec/1000000.0;
}

static int deterministic_enabled(void){
    const char *env = getenv("RMR_DETERMINISTIC");
    return env && strcmp(env, "1") == 0;
}

static void report_timestamp(struct tm *out_tm){
    if(deterministic_enabled()){
        memset(out_tm, 0, sizeof(*out_tm));
        out_tm->tm_year = 70;
        out_tm->tm_mon = 0;
        out_tm->tm_mday = 1;
        return;
    }

    time_t tt = time(NULL);
    struct tm *tm = gmtime(&tt);
    if(tm){
        *out_tm = *tm;
        return;
    }
    memset(out_tm, 0, sizeof(*out_tm));
    out_tm->tm_year = 70;
    out_tm->tm_mon = 0;
    out_tm->tm_mday = 1;
}

static int mkdir_p(const char *path){
    if(mkdir(path,0700)==0) return 0;
    if(errno==EEXIST) return 0;
    return -1;
}

static void usage(void){
    puts("pai bench --repeat N --out OUTDIR [--quiet] [--bytes N] -- <comando> [args...]");
    puts("  --quiet  : suprime stdout/stderr do comando benchmarkado");
    puts("  --bytes N: calcula throughput no report (use p/ hash de arquivo grande)");
}

static int run_self_cmd(int quiet, char *const argv_exec[]){
    pid_t pid = fork();
    if(pid < 0) return -1;

    if(pid == 0){
        if(quiet){
            int fd = open("/dev/null", O_WRONLY);
            if(fd >= 0){
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
        }
        execv(argv_exec[0], argv_exec);
        _exit(127);
    }

    int status = 0;
    if(waitpid(pid, &status, 0) < 0) return -1;
    if(WIFEXITED(status)) return WEXITSTATUS(status);
    return 128;
}

int pai_cmd_bench(int argc, char **argv){
    int repeat = 0;
    const char *out = NULL;
    int quiet = 0;
    long long bytes = -1;

    int sep = -1;
    for(int i=2;i<argc;i++){
        if(!strcmp(argv[i],"--repeat") && i+1<argc){ repeat = atoi(argv[++i]); continue; }
        if(!strcmp(argv[i],"--out") && i+1<argc){ out = argv[++i]; continue; }
        if(!strcmp(argv[i],"--quiet")){ quiet = 1; continue; }
        if(!strcmp(argv[i],"--bytes") && i+1<argc){ bytes = atoll(argv[++i]); continue; }
        if(!strcmp(argv[i],"--")){ sep = i; break; }
        if(!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")){ usage(); return 0; }
    }

    if(repeat <= 0 || !out || sep < 0 || sep+1 >= argc){
        usage();
        return 2;
    }

    if(mkdir_p(out)!=0){ perror("mkdir"); return 3; }

    char tsvpath[512], rptpath[512];
    snprintf(tsvpath,sizeof(tsvpath),"%s/bench.tsv",out);
    snprintf(rptpath,sizeof(rptpath),"%s/bench_report.txt",out);

    FILE *tsv = fopen(tsvpath,"w");
    if(!tsv){ perror("fopen"); return 4; }
    fprintf(tsv,"cmd\tok\tms\n");

    // monta argv para executar: ./pai <cmd> <args...>
    // o <cmd> é argv[sep+1], args seguintes idem
    // argv_exec[0] = "./pai" (default); se nao existir, usa argv[0] do processo
    const char *self = "./pai";
    if(access(self, X_OK)!=0) self = argv[0];

    int sub_argc = argc - (sep+1);
    // +2: self + NULL
    char **argv_exec = (char**)calloc((size_t)sub_argc + 2, sizeof(char*));
    if(!argv_exec){ fclose(tsv); return 5; }

    argv_exec[0] = (char*)self;
    for(int i=0;i<sub_argc;i++){
        argv_exec[i+1] = argv[sep+1+i];
    }
    argv_exec[sub_argc+1] = NULL;

    const char *cmd = argv[sep+1];

    double sum=0.0, mn=1e300, mx=0.0;
    int okcount=0;

    for(int i=0;i<repeat;i++){
        double t0 = now_ms();
        int rc = run_self_cmd(quiet, argv_exec);
        double t1 = now_ms();
        double dt = t1 - t0;

        int ok = (rc==0);
        if(ok) okcount++;

        fprintf(tsv,"%s\t%d\t%.3f\n", cmd, ok?1:0, dt);

        sum += dt;
        if(dt < mn) mn = dt;
        if(dt > mx) mx = dt;
    }

    free(argv_exec);
    fclose(tsv);

    double avg = sum / (double)repeat;

    struct tm tm;
    report_timestamp(&tm);

    FILE *rpt = fopen(rptpath,"w");
    if(!rpt){ perror("fopen"); return 6; }

    fprintf(rpt,"PAI BENCH v1\n");
    fprintf(rpt,"timestamp=%04d-%02d-%02dT%02d:%02d:%02dZ\n",
        tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(rpt,"cmd=%s\n", cmd);
    fprintf(rpt,"repeat=%d\n", repeat);
    fprintf(rpt,"ok=%d/%d\n", okcount, repeat);
    fprintf(rpt,"ms_avg=%.3f\n", avg);
    fprintf(rpt,"ms_min=%.3f\n", mn);
    fprintf(rpt,"ms_max=%.3f\n", mx);
    fprintf(rpt,"tsv=%s\n", tsvpath);

    if(bytes > 0){
        double sec = avg/1000.0;
        double bps = (sec>0.0) ? ((double)bytes / sec) : 0.0;
        fprintf(rpt,"bytes=%lld\n", bytes);
        fprintf(rpt,"MBps=%.3f\n", bps/1000000.0);
        fprintf(rpt,"MiBps=%.3f\n", bps/1048576.0);
    }

    fclose(rpt);

    printf("[OK] bench.tsv: %s\n", tsvpath);
    printf("[OK] bench_report.txt: %s\n", rptpath);
    return 0;
}
