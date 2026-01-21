#pragma once
#include "pai.h"

typedef struct {
    const char *base_dir;
    const char *out_dir;

    int include_hidden;
    int follow_symlinks;
    int max_depth;      // -1 unlimited
    long long max_size; // -1 unlimited

    // excludes (paths relativos ou absolutos)
    const char **excludes;
    int nexcludes;
} pai_scan_opts;

int pai_cmd_scan(int argc, char **argv);
int pai_scan_run(const pai_scan_opts *opt);
