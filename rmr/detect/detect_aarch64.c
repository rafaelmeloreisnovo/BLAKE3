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

#if defined(RMR_ARCH_AARCH64)

static uint64_t rmr_aarch64_read_id_aa64isar0_el1(void) {
  uint64_t value = 0;
  __asm__ volatile("mrs %0, ID_AA64ISAR0_EL1" : "=r"(value));
  return value;
}

static uint64_t rmr_aarch64_read_currentel(void) {
  uint64_t value = 0;
  __asm__ volatile("mrs %0, CurrentEL" : "=r"(value));
  return value;
}

void rmr_detect_cpu_caps(rmr_cpu_caps *out_caps) {
  if (!out_caps) {
    return;
  }

  out_caps->architecture = RMR_CPU_ARCH_AARCH64;
  out_caps->simd_extensions = 0;
#if defined(RMR_LITTLE_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_LITTLE;
#elif defined(RMR_BIG_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_BIG;
#else
  out_caps->endianness = RMR_ENDIAN_UNKNOWN;
#endif
  out_caps->register_width = 64;
  out_caps->execution_mode = RMR_EXECUTION_MODE_USER;
  out_caps->reserved = 0;

#if defined(RMR_AARCH64_ASSUME_PRIVILEGED)
  {
    uint64_t isar0 = rmr_aarch64_read_id_aa64isar0_el1();
    uint64_t currentel = rmr_aarch64_read_currentel();
    uint32_t advsimd_field = (uint32_t)((isar0 >> 20) & 0x0fu);

    if (advsimd_field != 0x0fu) {
      out_caps->simd_extensions |= RMR_SIMD_NEON;
    }

    switch ((currentel >> 2) & 0x3u) {
      case 1: out_caps->execution_mode = RMR_EXECUTION_MODE_KERNEL; break;
      case 2: out_caps->execution_mode = RMR_EXECUTION_MODE_HYPERVISOR; break;
      case 3: out_caps->execution_mode = RMR_EXECUTION_MODE_BAREMETAL; break;
      default: out_caps->execution_mode = RMR_EXECUTION_MODE_USER; break;
    }
  }
#else
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
  out_caps->simd_extensions |= RMR_SIMD_NEON;
#endif
#endif
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
