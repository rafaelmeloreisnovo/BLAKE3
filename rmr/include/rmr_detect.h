/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */
#ifndef RMR_DETECT_H
#define RMR_DETECT_H

#include <stdint.h>
#include "rmr_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
  RMR_CPU_ARCH_UNKNOWN = 0,
  RMR_CPU_ARCH_X86 = 1,
  RMR_CPU_ARCH_AARCH64 = 2,
  RMR_CPU_ARCH_ARM = 3,
  RMR_CPU_ARCH_RISCV = 4,
  RMR_CPU_ARCH_PPC = 5,
};

enum {
  RMR_ENDIAN_UNKNOWN = 0,
  RMR_ENDIAN_LITTLE = 1,
  RMR_ENDIAN_BIG = 2,
};

enum {
  RMR_EXECUTION_MODE_UNKNOWN = 0,
  RMR_EXECUTION_MODE_USER = 1,
  RMR_EXECUTION_MODE_KERNEL = 2,
  RMR_EXECUTION_MODE_HYPERVISOR = 3,
  RMR_EXECUTION_MODE_BAREMETAL = 4,
};

enum {
  RMR_SIMD_NONE = 0,
  RMR_SIMD_SSE2 = (1u << 0),
  RMR_SIMD_SSE41 = (1u << 1),
  RMR_SIMD_AVX2 = (1u << 2),
  RMR_SIMD_AVX512 = (1u << 3),
  RMR_SIMD_NEON = (1u << 4),
};

typedef struct rmr_cpu_caps {
  uint32_t architecture;
  uint32_t simd_extensions;
  uint8_t endianness;
  uint8_t register_width;
  uint8_t execution_mode;
  uint8_t reserved;
} rmr_cpu_caps;

void rmr_detect_cpu_caps(rmr_cpu_caps *out_caps);
const rmr_cpu_caps *rmr_get_cpu_caps(void);

#ifdef __cplusplus
}
#endif

#endif
