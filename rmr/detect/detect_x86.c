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

#if defined(RMR_ARCH_X86_64) || defined(RMR_ARCH_X86_32)

static void rmr_x86_cpuid(uint32_t leaf, uint32_t subleaf,
                          uint32_t *eax, uint32_t *ebx,
                          uint32_t *ecx, uint32_t *edx) {
#if defined(__i386__) && defined(__PIC__)
  uint32_t out_ebx;
  __asm__ volatile(
      "xchgl %%ebx, %1\n\t"
      "cpuid\n\t"
      "xchgl %%ebx, %1\n\t"
      : "=a"(*eax), "=&r"(out_ebx), "=c"(*ecx), "=d"(*edx)
      : "0"(leaf), "2"(subleaf)
      : "cc");
  *ebx = out_ebx;
#else
  __asm__ volatile(
      "cpuid"
      : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
      : "0"(leaf), "2"(subleaf)
      : "cc");
#endif
}

static uint64_t rmr_x86_xgetbv(uint32_t xcr) {
  uint32_t eax;
  uint32_t edx;
  __asm__ volatile(
      ".byte 0x0f, 0x01, 0xd0"
      : "=a"(eax), "=d"(edx)
      : "c"(xcr)
      : "cc");
  return ((uint64_t)edx << 32) | eax;
}

void rmr_detect_cpu_caps(rmr_cpu_caps *out_caps) {
  uint32_t eax = 0;
  uint32_t ebx = 0;
  uint32_t ecx = 0;
  uint32_t edx = 0;
  uint32_t max_leaf = 0;
  uint64_t xcr0 = 0;

  if (!out_caps) {
    return;
  }

  out_caps->architecture = RMR_CPU_ARCH_X86;
  out_caps->simd_extensions = 0;
#if defined(RMR_LITTLE_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_LITTLE;
#elif defined(RMR_BIG_ENDIAN)
  out_caps->endianness = RMR_ENDIAN_BIG;
#else
  out_caps->endianness = RMR_ENDIAN_UNKNOWN;
#endif
#if defined(RMR_ARCH_X86_64)
  out_caps->register_width = 64;
  out_caps->execution_mode = RMR_EXECUTION_MODE_USER;
#else
  out_caps->register_width = 32;
  out_caps->execution_mode = RMR_EXECUTION_MODE_USER;
#endif
  out_caps->reserved = 0;

  rmr_x86_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
  max_leaf = eax;
  if (max_leaf < 1u) {
    return;
  }

  rmr_x86_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
  if (edx & (1u << 26)) {
    out_caps->simd_extensions |= RMR_SIMD_SSE2;
  }
  if (ecx & (1u << 19)) {
    out_caps->simd_extensions |= RMR_SIMD_SSE41;
  }

  if ((ecx & (1u << 27)) && (ecx & (1u << 28))) {
    xcr0 = rmr_x86_xgetbv(0);
    if ((xcr0 & 0x6u) == 0x6u && max_leaf >= 7u) {
      rmr_x86_cpuid(7, 0, &eax, &ebx, &ecx, &edx);
      if (ebx & (1u << 5)) {
        out_caps->simd_extensions |= RMR_SIMD_AVX2;
      }
      if ((ebx & (1u << 16)) && ((xcr0 & 0xe6u) == 0xe6u)) {
        out_caps->simd_extensions |= RMR_SIMD_AVX512;
      }
    }
  }
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
