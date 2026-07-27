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
    /*
     * Returns a deterministic architecture-specific 64-bit hardware
     * fingerprint. AArch64 preserves the historical RMR AUTO-ID64 layout:
     * MIDR_EL1[31:0] in bits 63..32 and MPIDR_EL1[31:0] in bits 31..0.
     * x86_64 folds CPUID model/capability registers. This is not a PUF or a
     * guaranteed globally unique physical serial number.
     */
    rmr_u64 (*read_cpu_id_raw)(void);
    rmr_s64 (*raw_write)(int fd, const void* buf, rmr_u64 len);
    const char* backend_name;
} rmr_hwif_ops;

/*
 * Lazy one-time backend selection (bootstrap).
 * Thread-safe for concurrent callers; publishes backend pointer coherently.
 */
const rmr_hwif_ops* rmr_hwif_bootstrap(void);
/*
 * Lazy accessor: triggers bootstrap on first use.
 * Thread-safe; always returns a coherent backend pointer after publication.
 */
const rmr_hwif_ops* rmr_hwif_current(void);

#endif
