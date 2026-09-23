/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * Minimal hardware interface for RMR runtime dispatch.
 */
#ifndef RMR_HWIF_H
#define RMR_HWIF_H

#include "rmr_arch.h"
#include <stdint.h>

typedef uint64_t rmr_u64;
typedef int64_t rmr_s64;

enum {
    RMR_HWIF_CAP_TIME_HARDWARE = (1u << 0),
    RMR_HWIF_CAP_CPU_ID_HARDWARE = (1u << 1),
    RMR_HWIF_CAP_RAW_WRITE_DIRECT = (1u << 2),
    RMR_HWIF_CAP_PRIVILEGED_REGS = (1u << 3)
};

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
    uint32_t capability_flags;
    const char* backend_name;
} rmr_hwif_ops;

/*
 * capability_flags states where values come from; a backend name alone must
 * never be interpreted as proof that every operation is hardware-derived.
 *
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
