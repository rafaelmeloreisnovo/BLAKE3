/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "../include/rmr_detect.h"

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
  static uint8_t initialized = 0;

  if (!initialized) {
    rmr_detect_cpu_caps(&caps);
    initialized = 1;
  }

  return &caps;
}

#endif
