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

static double now_ms(void){ struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return (double)ts.tv_sec*1000.0 + (double)ts.tv_nsec/1000000.0; }
static int deterministic_enabled(void){ const char *env = getenv("RMR_DETERMINISTIC"); return env && strcmp(env, "1") == 0; }
static void report_timestamp(struct tm *out_tm){ if(deterministic_enabled()){ memset(out_tm,0,sizeof(*out_tm)); out_tm->tm_year=70; out_tm->tm_mon=0; out_tm->tm_mday=1; return;} time_t tt=time(NULL); struct tm *tm=gmtime(&tt); if(tm){*out_tm=*tm;return;} memset(out_tm,0,sizeof(*out_tm)); out_tm->tm_year=70; out_tm->tm_mon=0; out_tm->tm_mday=1; }
static int mkdir_p(const char *path){ if(mkdir(path,0700)==0) return 0; if(errno==EEXIST) return 0; return -1; }
static unsigned long long fnv1a64_update(unsigned long long h, const unsigned char *data, size_t n){ for(size_t i=0;i<n;i++){ h^=data[i]; h*=1099511628211ull; } return h; }
static unsigned long long fnv1a64_cstr(const char *s){ unsigned long long h=1469598103934665603ull; return fnv1a64_update(h,(const unsigned char*)s,strlen(s)); }
static int exec_capture(const char *cmd, char *out, size_t n){ FILE *fp=popen(cmd,"r"); if(!fp) return -1; if(!fgets(out,(int)n,fp)){ out[0]='\0'; pclose(fp); return -1; } out[strcspn(out,"\r\n")]='\0'; pclose(fp); return 0; }
static const char *arch_name(uint32_t arch){ switch(arch){ case RMR_CPU_ARCH_X86:return "x86"; case RMR_CPU_ARCH_AARCH64:return "aarch64"; case RMR_CPU_ARCH_ARM:return "arm"; case RMR_CPU_ARCH_RISCV:return "riscv"; case RMR_CPU_ARCH_PPC:return "ppc"; default:return "unknown"; }}
static void simd_to_str(uint32_t simd, char *buf, size_t n){ buf[0]='\0'; if(simd==0){snprintf(buf,n,"none");return;} int off=0; if(simd&RMR_SIMD_SSE2) off+=snprintf(buf+off,n-(size_t)off,"sse2,"); if(simd&RMR_SIMD_SSE41) off+=snprintf(buf+off,n-(size_t)off,"sse4.1,"); if(simd&RMR_SIMD_AVX2) off+=snprintf(buf+off,n-(size_t)off,"avx2,"); if(simd&RMR_SIMD_AVX512) off+=snprintf(buf+off,n-(size_t)off,"avx512,"); if(simd&RMR_SIMD_NEON) off+=snprintf(buf+off,n-(size_t)off,"neon,"); if(off>0 && buf[off-1]==',') buf[off-1]='\0'; }
static void usage(void){
    puts("pai bench --repeat N --out OUTDIR [--quiet] [--bytes N] [--metrics-store DIR] [--append|--new-session] -- <comando> [args...]");
}

static int run_self_cmd(int quiet, char *const argv_exec[]){ pid_t pid=fork(); if(pid<0) return -1; if(pid==0){ if(quiet){ int fd=open("/dev/null",O_WRONLY); if(fd>=0){dup2(fd,1);dup2(fd,2);close(fd);} } execv(argv_exec[0],argv_exec); _exit(127);} int status=0; if(waitpid(pid,&status,0)<0) return -1; if(WIFEXITED(status)) return WEXITSTATUS(status); return 128; }

int pai_cmd_bench(int argc, char **argv){
    int repeat=0,quiet=0,append_mode=0,new_session=0; const char *out=NULL,*metrics_store=NULL; long long bytes=-1; int sep=-1;
    for(int i=2;i<argc;i++){
        if(!strcmp(argv[i],"--repeat")&&i+1<argc){repeat=atoi(argv[++i]);continue;}
        if(!strcmp(argv[i],"--out")&&i+1<argc){out=argv[++i];continue;}
        if(!strcmp(argv[i],"--quiet")){quiet=1;continue;}
        if(!strcmp(argv[i],"--bytes")&&i+1<argc){bytes=atoll(argv[++i]);continue;}
        if(!strcmp(argv[i],"--metrics-store")&&i+1<argc){metrics_store=argv[++i];continue;}
        if(!strcmp(argv[i],"--append")){append_mode=1;continue;}
        if(!strcmp(argv[i],"--new-session")){new_session=1;continue;}
        if(!strcmp(argv[i],"--")){sep=i;break;}
        if(!strcmp(argv[i],"--help")||!strcmp(argv[i],"-h")){usage();return 0;}
    }
    if(append_mode && new_session){ fprintf(stderr,"[erro] use apenas --append ou --new-session\n"); return 2; }
    if(repeat<=0 || !out || sep<0 || sep+1>=argc){ usage(); return 2; }
    if(!metrics_store) metrics_store="rmr/benchmark_framework/output";
    if(mkdir_p(out)!=0){ perror("mkdir"); return 3; }
    if(mkdir_p(metrics_store)!=0){ perror("mkdir metrics_store"); return 3; }

    char tsvpath[512],rptpath[512],manifest[512],metricsj[512],summaryj[512],artifactdir[512];
    snprintf(tsvpath,sizeof(tsvpath),"%s/bench.tsv",out); snprintf(rptpath,sizeof(rptpath),"%s/bench_report.txt",out);
    snprintf(manifest,sizeof(manifest),"%s/run_manifest.json",metrics_store); snprintf(metricsj,sizeof(metricsj),"%s/metrics.jsonl",metrics_store);
    snprintf(summaryj,sizeof(summaryj),"%s/summary.json",metrics_store); snprintf(artifactdir,sizeof(artifactdir),"%s/artifacts",metrics_store);
    mkdir_p(artifactdir);

    FILE *tsv=fopen(tsvpath,"w"); if(!tsv){ perror("fopen"); return 4; } fprintf(tsv,"cmd\tok\tms\n");
    const char *self="./pai"; if(access(self,X_OK)!=0) self=argv[0];
    int sub_argc=argc-(sep+1); char **argv_exec=(char**)calloc((size_t)sub_argc+2,sizeof(char*)); if(!argv_exec){ fclose(tsv); return 5; }
    argv_exec[0]=(char*)self; for(int i=0;i<sub_argc;i++) argv_exec[i+1]=argv[sep+1+i]; argv_exec[sub_argc+1]=NULL;

    double *samples=(double*)calloc((size_t)repeat,sizeof(double)); if(!samples){ free(argv_exec); fclose(tsv); return 5; }
    double sum=0.0,mn=1e300,mx=0.0; int okcount=0; const char *cmd=argv[sep+1];
    unsigned long long run_chain=1469598103934665603ull, prev_run_hash=0;

    FILE *mj=fopen(metricsj, (append_mode&&!new_session)?"a":"w"); if(!mj){ perror("metrics.jsonl"); free(samples); free(argv_exec); fclose(tsv); return 6; }

    for(int i=0;i<repeat;i++){
        double t0=now_ms(); int rc=run_self_cmd(quiet,argv_exec); double dt=now_ms()-t0; int ok=(rc==0); if(ok) okcount++;
        samples[i]=dt; fprintf(tsv,"%s\t%d\t%.3f\n",cmd,ok?1:0,dt); sum+=dt; if(dt<mn) mn=dt; if(dt>mx) mx=dt;
        char run_blob[256]; snprintf(run_blob,sizeof(run_blob),"%d|%.3f|%d|%llu",i+1,dt,ok,prev_run_hash);
        run_chain = fnv1a64_update(run_chain,(const unsigned char*)run_blob,strlen(run_blob)); prev_run_hash=run_chain;
        fprintf(mj,"{\"run\":%d,\"ok\":%d,\"ms\":%.3f,\"prev_run_hash\":\"%016llx\",\"run_hash\":\"%016llx\"}\n",i+1,ok,dt,(unsigned long long)(i==0?0:prev_run_hash),(unsigned long long)run_chain);
    }
    fclose(mj); free(argv_exec); fclose(tsv);

    double avg=sum/(double)repeat, var=0.0; for(int i=0;i<repeat;i++){ double d=samples[i]-avg; var+=d*d; } var/= (double)repeat;
    for(int i=0;i<repeat;i++) for(int j=i+1;j<repeat;j++) if(samples[j]<samples[i]){double t=samples[i];samples[i]=samples[j];samples[j]=t;}
    double med = (repeat%2)?samples[repeat/2]:(samples[repeat/2-1]+samples[repeat/2])*0.5;
    int idx95 = (int)ceil(0.95*repeat)-1; if(idx95<0) idx95=0; if(idx95>=repeat) idx95=repeat-1; double p95=samples[idx95]; free(samples);

    struct tm tm; report_timestamp(&tm);
    FILE *rpt=fopen(rptpath,"w"); if(!rpt){ perror("fopen"); return 6; }
    fprintf(rpt,"PAI BENCH v1\ntimestamp=%04d-%02d-%02dT%02d:%02d:%02dZ\ncmd=%s\nrepeat=%d\nok=%d/%d\nms_avg=%.3f\nms_min=%.3f\nms_max=%.3f\ntsv=%s\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec,cmd,repeat,okcount,repeat,avg,mn,mx,tsvpath);
    if(bytes>0){ double sec=avg/1000.0,bps=(sec>0)?((double)bytes/sec):0.0; fprintf(rpt,"bytes=%lld\nMBps=%.3f\nMiBps=%.3f\n",bytes,bps/1e6,bps/1048576.0);} fclose(rpt);

    char commit[128]="unknown", cflags[256]="unknown", utc[64], simd[128], fp_in[17], fp_out[17], snap[17], host[128]="unknown";
    if(exec_capture("git rev-parse --short=12 HEAD",commit,sizeof(commit))!=0){}
    if(exec_capture("uname -m",host,sizeof(host))!=0){}
    snprintf(cflags,sizeof(cflags),"%s", getenv("CFLAGS")?getenv("CFLAGS"):"unknown");
    snprintf(utc,sizeof(utc),"%04d-%02d-%02dT%02d:%02d:%02dZ",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
    const rmr_cpu_caps *caps=rmr_get_cpu_caps(); simd_to_str(caps->simd_extensions,simd,sizeof(simd));

    unsigned long long inhash=fnv1a64_cstr(cmd); for(int i=sep+2;i<argc;i++) inhash=fnv1a64_update(inhash,(const unsigned char*)argv[i],strlen(argv[i]));
    unsigned long long outhash=fnv1a64_cstr(tsvpath); outhash=fnv1a64_update(outhash,(const unsigned char*)rptpath,strlen(rptpath));
    snprintf(fp_in,sizeof(fp_in),"%016llx",inhash); snprintf(fp_out,sizeof(fp_out),"%016llx",outhash); snprintf(snap,sizeof(snap),"%016llx",inhash^outhash);

    FILE *mf=fopen(manifest,"w"); if(mf){ fprintf(mf,"{\n  \"timestamp_utc\": \"%s\",\n  \"commit_hash\": \"%s\",\n  \"config\": {\"repeat\": %d, \"bytes\": %lld, \"quiet\": %d, \"command\": \"%s\"},\n  \"environment\": {\"cpu_arch\": \"%s\", \"host_arch\": \"%s\", \"simd_detected\": \"%s\", \"rmr_cpu_caps\": {\"register_width\": %u}, \"effective_cflags\": \"%s\"},\n  \"fingerprints\": {\"input_set\": \"%s\", \"snapshot_hash\": \"%s\", \"output_artifacts\": \"%s\"}\n}\n",utc,commit,repeat,bytes,quiet,cmd,arch_name(caps->architecture),host,simd,caps->register_width,cflags,fp_in,snap,fp_out); fclose(mf);}    
    FILE *sf=fopen(summaryj,"w"); if(sf){ fprintf(sf,"{\"min_ms\":%.3f,\"max_ms\":%.3f,\"median_ms\":%.3f,\"p95_ms\":%.3f,\"variance\":%.6f,\"history_compare\":{\"session\":\"%s\",\"path\":\"%s\"}}\n",mn,mx,med,p95,var,new_session?"new":"append",metricsj); fclose(sf);}    

    printf("[OK] bench.tsv: %s\n",tsvpath); printf("[OK] bench_report.txt: %s\n",rptpath);
    printf("[OK] run_manifest.json: %s\n",manifest); printf("[OK] metrics.jsonl: %s\n",metricsj); printf("[OK] summary.json: %s\n",summaryj);
    return 0;
}
