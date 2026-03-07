/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */

#include "include/rmr_hwif.h"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define RMR_HWIF_LOAD_PTR_ACQUIRE(v) atomic_load_explicit(&(v), memory_order_acquire)
#define RMR_HWIF_STORE_PTR_RELEASE(v, x) atomic_store_explicit(&(v), (x), memory_order_release)
#define RMR_HWIF_LOAD_STATE_ACQUIRE(v) atomic_load_explicit(&(v), memory_order_acquire)
#define RMR_HWIF_STORE_STATE_RELEASE(v, x) atomic_store_explicit(&(v), (x), memory_order_release)
#define RMR_HWIF_CAS_STATE_ACQ_REL(v, expected, desired) \
    atomic_compare_exchange_strong_explicit(&(v), &(expected), (desired), memory_order_acq_rel, memory_order_acquire)
typedef _Atomic(const rmr_hwif_ops*) rmr_hwif_ops_ptr_atomic_t;
typedef atomic_uchar rmr_hwif_init_state_t;
#else
#define RMR_HWIF_LOAD_PTR_ACQUIRE(v) __atomic_load_n(&(v), __ATOMIC_ACQUIRE)
#define RMR_HWIF_STORE_PTR_RELEASE(v, x) __atomic_store_n(&(v), (x), __ATOMIC_RELEASE)
#define RMR_HWIF_LOAD_STATE_ACQUIRE(v) __atomic_load_n(&(v), __ATOMIC_ACQUIRE)
#define RMR_HWIF_STORE_STATE_RELEASE(v, x) __atomic_store_n(&(v), (x), __ATOMIC_RELEASE)
#define RMR_HWIF_CAS_STATE_ACQ_REL(v, expected, desired) \
    __atomic_compare_exchange_n(&(v), &(expected), (desired), 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
typedef const rmr_hwif_ops* rmr_hwif_ops_ptr_atomic_t;
typedef unsigned char rmr_hwif_init_state_t;
#endif

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

static rmr_hwif_ops_ptr_atomic_t g_current = &g_fallback_ops;
static rmr_hwif_init_state_t g_bootstrap_state = 0;

const rmr_hwif_ops* rmr_hwif_bootstrap(void) {
    unsigned char expected = 0;
    unsigned char state = RMR_HWIF_LOAD_STATE_ACQUIRE(g_bootstrap_state);

    /*
     * Lazy bootstrap, thread-safe, lock-free.
     * Contract: publication of g_current is release-ordered; readers using
     * rmr_hwif_current observe a coherent backend pointer after bootstrap.
     */
    if (state == 2) {
        return RMR_HWIF_LOAD_PTR_ACQUIRE(g_current);
    }

    if (RMR_HWIF_CAS_STATE_ACQ_REL(g_bootstrap_state, expected, 1)) {
        const rmr_hwif_ops* selected = &g_fallback_ops;
#if defined(RMR_ARCH_AARCH64)
        if (rmr_hwif_aarch64_backend_probe() == 1) {
            selected = &g_aarch64_ops;
        }
#endif
#if defined(RMR_ARCH_X86_64)
        if (rmr_hwif_x86_64_backend_probe() == 1) {
            selected = &g_x86_64_ops;
        }
#endif
        RMR_HWIF_STORE_PTR_RELEASE(g_current, selected);
        RMR_HWIF_STORE_STATE_RELEASE(g_bootstrap_state, 2);
        return selected;
    }

    while (RMR_HWIF_LOAD_STATE_ACQUIRE(g_bootstrap_state) != 2) {
    }

    return RMR_HWIF_LOAD_PTR_ACQUIRE(g_current);
}

const rmr_hwif_ops* rmr_hwif_current(void) {
    if (RMR_HWIF_LOAD_STATE_ACQUIRE(g_bootstrap_state) != 2) {
        return rmr_hwif_bootstrap();
    }
    return RMR_HWIF_LOAD_PTR_ACQUIRE(g_current);
}
