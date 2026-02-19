/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "include/rmr_hwif.h"

static rmr_u64 g_fallback_tick = 0xC0DEC0DEC0DEC0DEUL;

static rmr_u64 fallback_time_raw(void) {
    g_fallback_tick += 0x9E3779B97F4A7C15UL;
    return g_fallback_tick;
}

static rmr_u64 fallback_cpu_id_raw(void) {
    return 0x524D525F46414C4CUL; /* "RMR_FALL" */
}

static rmr_s64 fallback_raw_write(int fd, const void* buf, rmr_u64 len) {
    (void)fd;
    (void)buf;
    return (rmr_s64)len;
}

static const rmr_hwif_ops g_fallback_ops = {
    fallback_time_raw,
    fallback_cpu_id_raw,
    fallback_raw_write,
    "c_fallback"
};

#if defined(RMR_ARCH_AARCH64)
extern rmr_u64 rmr_hwif_aarch64_time_raw(void);
extern rmr_u64 rmr_hwif_aarch64_cpu_id_raw(void);
extern rmr_s64 rmr_hwif_aarch64_raw_write(int fd, const void* buf, rmr_u64 len);
extern rmr_u64 rmr_hwif_aarch64_backend_probe(void);

static const rmr_hwif_ops g_aarch64_ops = {
    rmr_hwif_aarch64_time_raw,
    rmr_hwif_aarch64_cpu_id_raw,
    rmr_hwif_aarch64_raw_write,
    "aarch64_asm"
};
#endif

#if defined(RMR_ARCH_X86_64)
extern rmr_u64 rmr_hwif_x86_64_time_raw(void);
extern rmr_u64 rmr_hwif_x86_64_cpu_id_raw(void);
extern rmr_s64 rmr_hwif_x86_64_raw_write(int fd, const void* buf, rmr_u64 len);
extern rmr_u64 rmr_hwif_x86_64_backend_probe(void);

static const rmr_hwif_ops g_x86_64_ops = {
    rmr_hwif_x86_64_time_raw,
    rmr_hwif_x86_64_cpu_id_raw,
    rmr_hwif_x86_64_raw_write,
    "x86_64_asm"
};
#endif

static const rmr_hwif_ops* g_current = &g_fallback_ops;

const rmr_hwif_ops* rmr_hwif_bootstrap(void) {
#if defined(RMR_ARCH_AARCH64)
    if (rmr_hwif_aarch64_backend_probe() == 1) {
        g_current = &g_aarch64_ops;
        return g_current;
    }
#endif
#if defined(RMR_ARCH_X86_64)
    if (rmr_hwif_x86_64_backend_probe() == 1) {
        g_current = &g_x86_64_ops;
        return g_current;
    }
#endif
    g_current = &g_fallback_ops;
    return g_current;
}

const rmr_hwif_ops* rmr_hwif_current(void) {
    return g_current;
}
