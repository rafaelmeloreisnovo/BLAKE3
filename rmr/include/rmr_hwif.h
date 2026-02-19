/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Minimal hardware interface for RMR runtime dispatch.
 */
#ifndef RMR_HWIF_H
#define RMR_HWIF_H

#include "rmr_arch.h"

typedef unsigned long rmr_u64;
typedef long rmr_s64;

typedef struct rmr_hwif_ops {
    rmr_u64 (*read_time_raw)(void);
    rmr_u64 (*read_cpu_id_raw)(void);
    rmr_s64 (*raw_write)(int fd, const void* buf, rmr_u64 len);
    const char* backend_name;
} rmr_hwif_ops;

const rmr_hwif_ops* rmr_hwif_bootstrap(void);
const rmr_hwif_ops* rmr_hwif_current(void);

#endif
