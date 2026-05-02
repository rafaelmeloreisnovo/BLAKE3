/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai.h"
#include "../hwif/include/rmr_detect.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define PAI_BENCH_MAX_REPEAT 4096

static double now_ms(void){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return (double)ts.tv_sec*1000.0 + (double)ts.tv_nsec/1000000.0; }
static int env_flag(const char *name){ const char *e=getenv(name); return e && strcmp(e,"1")==0; }
static long env_long_or(const char *name, long fallback){
    const char *e=getenv(name); char *end=NULL; long v;
    if(!e || !*e) return fallback;
    errno=0; v=strtol(e,&end,10);
    if(errno!=0 || end==e) return fallback;
    return v;
}
static int deterministic_enabled(void){ const char *e=getenv("RMR_DETERMINISTIC"); return e && strcmp(e,"1")==0; }
static void report_timestamp(struct tm *tm_out){
    if(deterministic_enabled()){ memset(tm_out,0,sizeof(*tm_out)); tm_out->tm_year=70; tm_out->tm_mon=0; tm_out->tm_mday=1; return; }
    time_t t=time(NULL); struct tm *pt=gmtime(&t);
    if(pt){ *tm_out=*pt; return; }
    memset(tm_out,0,sizeof(*tm_out)); tm_out->tm_year=70; tm_out->tm_mon=0; tm_out->tm_mday=1;
}
static int mkdir_p(const char *p){ if(mkdir(p,0700)==0) return 0; return errno==EEXIST?0:-1; }
static int run_self_cmd(int quiet, char *const argv_exec[]){
    pid_t pid=fork(); if(pid<0) return -1;
    if(pid==0){ if(quiet){ int fd=open("/dev/null",O_WRONLY); if(fd>=0){ dup2(fd,1); dup2(fd,2); close(fd);} } execv(argv_exec[0],argv_exec); _exit(127);} 
    int st=0; if(waitpid(pid,&st,0)<0) return -1; if(WIFEXITED(st)) return WEXITSTATUS(st); return 128;
}

int pai_cmd_bench(int argc, char **argv){
    int repeat=0, quiet=0, append_mode=0, new_session=0, sep=-1;
    long long bytes=-1;
    const char *out=NULL, *metrics_store=NULL;

    for(int i=2;i<argc;i++){
        if(!strcmp(argv[i],"--repeat") && i+1<argc){ repeat=atoi(argv[++i]); continue; }
        if(!strcmp(argv[i],"--out") && i+1<argc){ out=argv[++i]; continue; }
        if(!strcmp(argv[i],"--quiet")){ quiet=1; continue; }
        if(!strcmp(argv[i],"--bytes") && i+1<argc){ bytes=atoll(argv[++i]); continue; }
        if(!strcmp(argv[i],"--metrics-store") && i+1<argc){ metrics_store=argv[++i]; continue; }
        if(!strcmp(argv[i],"--append")){ append_mode=1; continue; }
        if(!strcmp(argv[i],"--new-session")){ new_session=1; continue; }
        if(!strcmp(argv[i],"--")){ sep=i; break; }
        if(!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")){
            puts("pai bench --repeat N --out OUTDIR [--quiet] [--bytes N] [--metrics-store DIR] [--append|--new-session] -- <comando> [args...]");
            return 0;
        }
    }

    if(append_mode && new_session){ fprintf(stderr,"[erro] use apenas --append ou --new-session\n"); return 2; }
    if(repeat<=0 || repeat>PAI_BENCH_MAX_REPEAT || !out || sep<0 || sep+1>=argc){ fprintf(stderr,"[erro] argumentos invalidos\n"); return 2; }
    if(!metrics_store) metrics_store="rmr/benchmark_framework/output";
    if(mkdir_p(out)!=0){ perror("mkdir out"); return 3; }
    if(mkdir_p(metrics_store)!=0){ perror("mkdir metrics-store"); return 3; }

    char tsv[512], rpt[512], mf[512], mj[512], sj[512];
    snprintf(tsv,sizeof(tsv),"%s/bench.tsv",out);
    snprintf(rpt,sizeof(rpt),"%s/bench_report.txt",out);
    snprintf(mf,sizeof(mf),"%s/run_manifest.json",metrics_store);
    snprintf(mj,sizeof(mj),"%s/metrics.jsonl",metrics_store);
    snprintf(sj,sizeof(sj),"%s/summary.json",metrics_store);

    FILE *ft=fopen(tsv,"w"); if(!ft){ perror("fopen bench.tsv"); return 4; }
    fprintf(ft,"cmd\tok\tms\n");

    const char *self="./pai"; if(access(self,X_OK)!=0) self=argv[0];
    char *xv[256]; int sub=argc-(sep+1); if(sub+2>256){ fclose(ft); return 5; }
    xv[0]=(char*)self; for(int i=0;i<sub;i++) xv[i+1]=argv[sep+1+i]; xv[sub+1]=NULL;

    double sample[PAI_BENCH_MAX_REPEAT];
    double sum=0.0, mn=1e300, mx=0.0;
    int okc=0;
    unsigned long long chain=1469598103934665603ull, prev=0ull;

    FILE *fm=fopen(mj,(append_mode && !new_session)?"a":"w"); if(!fm){ fclose(ft); perror("fopen metrics.jsonl"); return 6; }
    for(int i=0;i<repeat;i++){
        double t0=now_ms(); int rc=run_self_cmd(quiet,xv); double dt=now_ms()-t0; int ok=(rc==0);
        sample[i]=dt; sum+=dt; if(dt<mn) mn=dt; if(dt>mx) mx=dt; if(ok) okc++;
        fprintf(ft,"%s\t%d\t%.3f\n",argv[sep+1],ok?1:0,dt);
        char line[256]; snprintf(line,sizeof(line),"%d|%.3f|%d|%016llx",i+1,dt,ok,(unsigned long long)prev);
        for(size_t k=0;k<strlen(line);k++){ chain^=(unsigned char)line[k]; chain*=1099511628211ull; }
        fprintf(fm,"{\"run\":%d,\"ok\":%d,\"ms\":%.3f,\"prev_run_hash\":\"%016llx\",\"run_hash\":\"%016llx\"}\n",i+1,ok,dt,(unsigned long long)prev,(unsigned long long)chain);
        prev=chain;
    }
    fclose(ft); fclose(fm);

    double avg=sum/(double)repeat, var=0.0;
    for(int i=0;i<repeat;i++){ double d=sample[i]-avg; var+=d*d; }
    var/=(double)repeat;
    for(int i=0;i<repeat;i++) for(int j=i+1;j<repeat;j++) if(sample[j]<sample[i]){ double t=sample[i]; sample[i]=sample[j]; sample[j]=t; }
    double med=(repeat%2)?sample[repeat/2]:(sample[(repeat/2)-1]+sample[repeat/2])*0.5;
    int p95i=(int)ceil(0.95*(double)repeat)-1; if(p95i<0)p95i=0; if(p95i>=repeat)p95i=repeat-1;

    struct tm tm; report_timestamp(&tm);
    FILE *fr=fopen(rpt,"w"); if(!fr){ perror("fopen bench_report"); return 6; }
    fprintf(fr,"PAI BENCH v1\ntimestamp=%04d-%02d-%02dT%02d:%02d:%02dZ\ncmd=%s\nrepeat=%d\nok=%d/%d\nms_avg=%.3f\nms_min=%.3f\nms_max=%.3f\ntsv=%s\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,argv[sep+1],repeat,okc,repeat,avg,mn,mx,tsv);
    if(bytes>0){ double sec=avg/1000.0,bps=(sec>0.0)?((double)bytes/sec):0.0; fprintf(fr,"bytes=%lld\nMBps=%.3f\nMiBps=%.3f\n",bytes,bps/1e6,bps/1048576.0); }
    fclose(fr);

    const rmr_cpu_caps *c=rmr_get_cpu_caps();
    char simd[64]="none";
    if(c->simd_extensions){
        simd[0]='\0';
        if(c->simd_extensions&RMR_SIMD_SSE2) strcat(simd,"sse2,");
        if(c->simd_extensions&RMR_SIMD_SSE41) strcat(simd,"sse4.1,");
        if(c->simd_extensions&RMR_SIMD_AVX2) strcat(simd,"avx2,");
        if(c->simd_extensions&RMR_SIMD_AVX512) strcat(simd,"avx512,");
        if(c->simd_extensions&RMR_SIMD_NEON) strcat(simd,"neon,");
        if(simd[0]) simd[strlen(simd)-1]='\0';
    }

    unsigned long long inhash=1469598103934665603ull, outhash=1469598103934665603ull;
    for(size_t i=0;i<strlen(argv[sep+1]);i++){ inhash^=(unsigned char)argv[sep+1][i]; inhash*=1099511628211ull; }
    for(int i=sep+2;i<argc;i++) for(size_t k=0;k<strlen(argv[i]);k++){ inhash^=(unsigned char)argv[i][k]; inhash*=1099511628211ull; }
    for(size_t i=0;i<strlen(tsv);i++){ outhash^=(unsigned char)tsv[i]; outhash*=1099511628211ull; }
    for(size_t i=0;i<strlen(rpt);i++){ outhash^=(unsigned char)rpt[i]; outhash*=1099511628211ull; }

    const int gov_ntp_enabled = env_flag("RMR_GOV_NTP_ENABLED");
    const int gov_icmp_probe_enabled = env_flag("RMR_GOV_ICMP_PROBE_ENABLED");
    const int gov_jitter_sampling_enabled = env_flag("RMR_GOV_JITTER_SAMPLING_ENABLED");
    const int gov_offline_deterministic = env_flag("RMR_GOV_OFFLINE_DETERMINISTIC") || deterministic_enabled();
    const long gov_clock_sync_timeout_ms = env_long_or("RMR_GOV_CLOCK_SYNC_TIMEOUT_MS", 0);
    const long gov_icmp_probe_timeout_ms = env_long_or("RMR_GOV_ICMP_PROBE_TIMEOUT_MS", 0);
    const long gov_jitter_sample_window_ms = env_long_or("RMR_GOV_JITTER_SAMPLE_WINDOW_MS", 0);
    const long gov_telemetry_rate_limit_per_minute = env_long_or("RMR_GOV_TELEMETRY_RATE_LIMIT_PER_MINUTE", 0);
    const long gov_clock_sync_ms = env_long_or("RMR_GOV_LAST_CLOCK_SYNC_OFFSET_MS", 0);
    const long gov_icmp_rtt_ms = env_long_or("RMR_GOV_LAST_ICMP_RTT_MS", 0);
    const long gov_jitter_ppm = env_long_or("RMR_GOV_LAST_JITTER_PPM", 0);

    FILE *fman=fopen(mf,"w");
    if(fman){
        fprintf(fman,"{\n");
        fprintf(fman,"  \"timestamp_utc\": \"%04d-%02d-%02dT%02d:%02d:%02dZ\",\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        fprintf(fman,"  \"commit_hash\": \"%s\",\n", getenv("RMR_COMMIT_HASH")?getenv("RMR_COMMIT_HASH"):"unknown");
        fprintf(fman,"  \"config\": {\"repeat\": %d, \"bytes\": %lld, \"quiet\": %d, \"command\": \"%s\"},\n",repeat,bytes,quiet,argv[sep+1]);
        fprintf(fman,"  \"environment\": {\"cpu_arch\": %u, \"simd_detected\": \"%s\", \"rmr_cpu_caps\": {\"register_width\": %u}, \"build_profile\": \"%s\", \"effective_cflags\": \"%s\", \"effective_ldflags\": \"%s\"},\n",c->architecture,simd,c->register_width,getenv("RMR_BUILD_PROFILE")?getenv("RMR_BUILD_PROFILE"):"unknown",getenv("RMR_FINAL_CFLAGS")?getenv("RMR_FINAL_CFLAGS"):(getenv("CFLAGS")?getenv("CFLAGS"):"unknown"),getenv("RMR_FINAL_LDFLAGS")?getenv("RMR_FINAL_LDFLAGS"):"unknown");
        fprintf(fman,"  \"governance\": {\"telemetry\": {\"ntp_enabled\": %d, \"icmp_probe_enabled\": %d, \"jitter_sampling_enabled\": %d, \"offline_deterministic\": %d, \"clock_sync_timeout_ms\": %ld, \"icmp_probe_timeout_ms\": %ld, \"jitter_sample_window_ms\": %ld, \"telemetry_rate_limit_per_minute\": %ld}, \"metadata\": {\"clock_sync_ms\": %ld, \"icmp_rtt_ms\": %ld, \"jitter_ppm\": %ld}},\n",gov_ntp_enabled,gov_icmp_probe_enabled,gov_jitter_sampling_enabled,gov_offline_deterministic,gov_clock_sync_timeout_ms,gov_icmp_probe_timeout_ms,gov_jitter_sample_window_ms,gov_telemetry_rate_limit_per_minute,gov_clock_sync_ms,gov_icmp_rtt_ms,gov_jitter_ppm);
        fprintf(fman,"  \"fingerprints\": {\"input_set\": \"%016llx\", \"snapshot_hash\": \"%016llx\", \"output_artifacts\": \"%016llx\"}\n",(unsigned long long)inhash,(unsigned long long)(inhash^outhash),(unsigned long long)outhash);
        fprintf(fman,"}\n");
        fclose(fman);
    }

    FILE *fs=fopen(sj,"w");
    if(fs){
        fprintf(fs,"{\"min_ms\":%.3f,\"max_ms\":%.3f,\"median_ms\":%.3f,\"p95_ms\":%.3f,\"variance\":%.6f,\"history_compare\":{\"session\":\"%s\",\"store\":\"%s\",\"profile\":\"%s\"}}\n",mn,mx,med,sample[p95i],var,new_session?"new":"append",metrics_store,argv[sep+1]);
        fclose(fs);
    }

    printf("[OK] bench.tsv: %s\n",tsv);
    printf("[OK] bench_report.txt: %s\n",rpt);
    printf("[OK] run_manifest.json: %s\n",mf);
    printf("[OK] metrics.jsonl: %s\n",mj);
    printf("[OK] summary.json: %s\n",sj);
    return 0;
}
