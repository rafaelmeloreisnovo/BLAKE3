#pragma once
#include <stddef.h>
#include <stdint.h>

#ifndef PAI_MAX_PATH
#define PAI_MAX_PATH 4096
#endif

// --- core config ---
typedef struct {
    const char *base_path;
    const char *out_dir;

    // scan/filter
    int include_hidden;
    int follow_symlinks;
    int max_depth;      // -1 unlimited
    int64_t max_size;   // -1 unlimited
} pai_cfg_t;

// --- tiny helpers ---
void pai_die(const char *msg);
void pai_mkdir_p(const char *path);

// --- cli ---
int pai_main(int argc, char **argv);
