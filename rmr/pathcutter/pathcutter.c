#define _XOPEN_SOURCE 700

#include "pathcutter.h"

#include "../include/rmr_lowlevel.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../core/pai.h"

void rmr_pc_die(const char *msg) {
    perror(msg);
    exit(1);
}

int rmr_pc_mkdir_p(const char *path) {
    char tmp[PAI_MAX_PATH];
    size_t len = strlen(path);
    if (len == 0 || len >= sizeof(tmp)) {
        errno = EINVAL;
        fprintf(stderr, "mkdir_p: path invalido\n");
        return -1;
    }
    memcpy(tmp, path, len + 1);
    if (tmp[len - 1] == '/') tmp[len - 1] = 0;

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return -1;
    return 0;
}

void *rmr_pc_xmalloc(size_t n) {
    void *p = rmr_ll_malloc(n);
    if (!p) {
        perror("malloc");
        exit(2);
    }
    memset(p, 0, n);
    return p;
}

void rmr_pc_xfree(void *ptr) {
    rmr_ll_free(ptr);
}
