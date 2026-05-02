/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "pai.h"
#include "../pathcutter/pathcutter.h"

void pai_die(const char *msg) {
    rmr_pc_die(msg);
}

int pai_mkdir_p(const char *path) {
    return rmr_pc_mkdir_p(path);
}

void *pai_xmalloc(size_t n) {
    return rmr_pc_xmalloc(n);
}

void pai_xfree(void *ptr) {
    rmr_pc_xfree(ptr);
}
