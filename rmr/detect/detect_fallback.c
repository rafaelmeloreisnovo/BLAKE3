/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#include "../include/rmr_detect.h"

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
  static uint8_t initialized = 0;

  if (!initialized) {
    rmr_detect_cpu_caps(&caps);
    initialized = 1;
  }

  return &caps;
}

#endif
