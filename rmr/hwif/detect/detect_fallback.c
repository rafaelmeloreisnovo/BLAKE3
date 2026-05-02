/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "../include/rmr_detect.h"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_ATOMICS__)
#include <stdatomic.h>
#define RMR_INIT_LOAD_ACQUIRE(v) atomic_load_explicit(&(v), memory_order_acquire)
#define RMR_INIT_STORE_RELEASE(v, x) atomic_store_explicit(&(v), (x), memory_order_release)
#define RMR_INIT_CAS_ACQ_REL(v, expected, desired) \
  atomic_compare_exchange_strong_explicit(&(v), &(expected), (desired), memory_order_acq_rel, memory_order_acquire)
typedef atomic_uchar rmr_init_state_t;
#else
#define RMR_INIT_LOAD_ACQUIRE(v) __atomic_load_n(&(v), __ATOMIC_ACQUIRE)
#define RMR_INIT_STORE_RELEASE(v, x) __atomic_store_n(&(v), (x), __ATOMIC_RELEASE)
#define RMR_INIT_CAS_ACQ_REL(v, expected, desired) \
  __atomic_compare_exchange_n(&(v), &(expected), (desired), 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)
typedef uint8_t rmr_init_state_t;
#endif

#if !defined(RMR_ARCH_X86_64) && !defined(RMR_ARCH_X86_32) && !defined(RMR_ARCH_AARCH64)

void rmr_detect_cpu_caps(rmr_cpu_caps *out_caps) {
  if (!out_caps) {
    return;
  }

#if defined(RMR_ARCH_ARM)
  out_caps->architecture = RMR_CPU_ARCH_ARM;
#elif defined(RMR_ARCH_RISCV)
  out_caps->architecture = RMR_CPU_ARCH_RISCV;
#elif defined(RMR_ARCH_PPC64) || defined(RMR_ARCH_PPC32)
  out_caps->architecture = RMR_CPU_ARCH_PPC;
#else
  out_caps->architecture = RMR_CPU_ARCH_UNKNOWN;
#endif

  out_caps->simd_extensions = 0;
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
  out_caps->simd_extensions |= RMR_SIMD_NEON;
#endif

#if defined(RMR_LITTLE_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_LITTLE;
#elif defined(RMR_BIG_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_BIG;
#else
  out_caps->endianness = RMR_ENDIAN_UNKNOWN;
#endif

  out_caps->register_width = (uint8_t)(sizeof(void *) * 8u);
  out_caps->execution_mode = RMR_EXECUTION_MODE_USER;
  out_caps->reserved = 0;
}

const rmr_cpu_caps *rmr_get_cpu_caps(void) {
  static rmr_cpu_caps caps;
  static rmr_init_state_t init_state = 0;
  uint8_t expected = 0;
  uint8_t state = RMR_INIT_LOAD_ACQUIRE(init_state);

  /*
   * Lazy initialization, thread-safe, lock-free.
   * Contract: the returned pointer is stable and never observes a partially
   * initialized structure.
   */
  if (state == 2) {
    return &caps;
  }

  if (RMR_INIT_CAS_ACQ_REL(init_state, expected, 1)) {
    rmr_detect_cpu_caps(&caps);
    RMR_INIT_STORE_RELEASE(init_state, 2);
    return &caps;
  }

  while (RMR_INIT_LOAD_ACQUIRE(init_state) != 2) {
  }

  return &caps;
}

#endif
