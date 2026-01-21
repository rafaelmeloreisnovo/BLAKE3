#include "pai_toroid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    int nu, nv;
    float R, r;
    const char *tex;
    const char *out;
} tor_opt;

static void *xmalloc(size_t n){
    void *p = malloc(n);
    if(!p){ perror("malloc"); exit(2); }
    memset(p,0,n);
    return p;
}

static unsigned char* load_pgm(const char *path, int *w, int *h){
    FILE *f = fopen(path,"rb");
    if(!f){ perror(path); return NULL; }

    char magic[3];
    fscanf(f,"%2s",magic);
    if(strcmp(magic,"P5")!=0){ fclose(f); return NULL; }

    int maxv;
    fscanf(f,"%d %d %d",w,h,&maxv);
    (void)fgetc(f); // newline

    const size_t tw = (size_t)(*w);
    const size_t th = (size_t)(*h);
    unsigned char *buf = xmalloc(tw * th);
    (void)fread(buf, 1, tw * th, f);
    fclose(f);
    return buf;
}

int pai_cmd_toroid(int argc, char **argv){
    tor_opt o;
    o.nu = 256;
    o.nv = 128;
    o.R  = 1.0f;
    o.r  = 0.35f;
    o.tex = NULL;
    o.out = "out_toroid";

    for(int i=2;i<argc;i++){
        if(!strcmp(argv[i],"--nu") && i+1<argc) o.nu = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--nv") && i+1<argc) o.nv = atoi(argv[++i]);
        else if(!strcmp(argv[i],"--R") && i+1<argc) o.R = (float)atof(argv[++i]);
        else if(!strcmp(argv[i],"--r") && i+1<argc) o.r = (float)atof(argv[++i]);
        else if(!strcmp(argv[i],"--tex") && i+1<argc) o.tex = argv[++i];
        else if(!strcmp(argv[i],"--out") && i+1<argc) o.out = argv[++i];
    }

    if(!o.tex){
        fprintf(stderr,"uso: pai toroid --tex out_geom/geom.pgm [--out out_toroid]\n");
        return 1;
    }

    int tw,th;
    unsigned char *tex = load_pgm(o.tex,&tw,&th);
    if(!tex){ fprintf(stderr,"erro lendo textura\n"); return 2; }

    if (pai_mkdir_p(o.out) != 0) {
        perror("mkdir");
        free(tex);
        return 3;
    }

    char objpath[1024], mtlpath[1024], texpath[1024];
    snprintf(objpath,sizeof(objpath),"%s/toroid.obj",o.out);
    snprintf(mtlpath,sizeof(mtlpath),"%s/toroid.mtl",o.out);
    snprintf(texpath,sizeof(texpath),"%s/toroid_texture.pgm",o.out);

    // copia textura
    FILE *ft = fopen(texpath,"wb");
    if (!ft) {
        perror("toroid_texture");
        free(tex);
        return 4;
    }
    fprintf(ft,"P5\n%d %d\n255\n",tw,th);
    fwrite(tex, 1, (size_t)tw * (size_t)th, ft);
    fclose(ft);

    FILE *obj = fopen(objpath,"wb");
    if (!obj) {
        perror("toroid_obj");
        free(tex);
        return 5;
    }
    FILE *mtl = fopen(mtlpath,"wb");
    if (!mtl) {
        perror("toroid_mtl");
        fclose(obj);
        free(tex);
        return 6;
    }

    fprintf(mtl,"newmtl torus\nmap_Kd toroid_texture.pgm\n");
    fprintf(obj,"mtllib toroid.mtl\nusemtl torus\n");

    // vértices
    for(int j=0;j<=o.nv;j++){
        const float two_pi = (float)(2.0 * M_PI);
        float v = ((float)j/(float)o.nv) * two_pi;
        for(int i=0;i<=o.nu;i++){
            float u = ((float)i/(float)o.nu) * two_pi;

            float x = (o.R + o.r*cosf(v)) * cosf(u);
            float y = (o.R + o.r*cosf(v)) * sinf(u);
            float z = o.r * sinf(v);

            fprintf(obj,"v %f %f %f\n",x,y,z);
            fprintf(obj,"vt %f %f\n",(float)i/(float)o.nu,1.0f-(float)j/(float)o.nv);
        }
    }

    int cols = o.nu + 1;

    for(int j=0;j<o.nv;j++){
        for(int i=0;i<o.nu;i++){
            int a = j*cols + i + 1;
            int b = a + 1;
            int c = a + cols;
            int d = c + 1;
            fprintf(obj,"f %d/%d %d/%d %d/%d\n",a,a,c,c,b,b);
            fprintf(obj,"f %d/%d %d/%d %d/%d\n",b,b,c,c,d,d);
        }
    }

    fclose(obj);
    fclose(mtl);
    free(tex);

    printf("[OK] obj: %s\n",objpath);
    printf("[OK] mtl: %s\n",mtlpath);
    printf("[OK] texture: %s\n",texpath);
    return 0;
}
