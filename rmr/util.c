#define _XOPEN_SOURCE 700
#include "pai.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void pai_die(const char *msg) {
    perror(msg);
    exit(1);
}

int pai_mkdir_p(const char *path) {
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
