/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */
#ifndef RMR_DISPATCH_H
#define RMR_DISPATCH_H

#include "rmr_arch.h"
#include "rmr_detect.h"

// Compile-time feature flags remain available as conservative fallback.
#if defined(RMR_ARCH_X86_64) || defined(RMR_ARCH_X86_32)
#define RMR_HAS_X86 1
#else
#define RMR_HAS_X86 0
#endif

#if defined(RMR_ARCH_AARCH64) || defined(RMR_ARCH_ARM)
#define RMR_HAS_ARM 1
#else
#define RMR_HAS_ARM 0
#endif

#if RMR_HAS_X86 && defined(__SSE2__)
#define RMR_COMPILETIME_HAS_SSE2 1
#else
#define RMR_COMPILETIME_HAS_SSE2 0
#endif

#if RMR_HAS_X86 && defined(__SSE4_1__)
#define RMR_COMPILETIME_HAS_SSE41 1
#else
#define RMR_COMPILETIME_HAS_SSE41 0
#endif

#if RMR_HAS_X86 && defined(__AVX2__)
#define RMR_COMPILETIME_HAS_AVX2 1
#else
#define RMR_COMPILETIME_HAS_AVX2 0
#endif

#if RMR_HAS_X86 && defined(__AVX512F__)
#define RMR_COMPILETIME_HAS_AVX512 1
#else
#define RMR_COMPILETIME_HAS_AVX512 0
#endif

#if RMR_HAS_ARM && (defined(__ARM_NEON) || defined(__ARM_NEON__))
#define RMR_COMPILETIME_HAS_NEON 1
#else
#define RMR_COMPILETIME_HAS_NEON 0
#endif

static inline int rmr_dispatch_has_simd(uint32_t simd_bit, int fallback_value) {
#if defined(RMR_DISABLE_RUNTIME_DETECT)
  (void)simd_bit;
  return fallback_value;
#else
  const rmr_cpu_caps *caps = rmr_get_cpu_caps();
  if (!caps) {
    return fallback_value;
  }
  return (caps->simd_extensions & simd_bit) != 0u;
#endif
}

#define RMR_HAS_SSE2 rmr_dispatch_has_simd(RMR_SIMD_SSE2, RMR_COMPILETIME_HAS_SSE2)
#define RMR_HAS_SSE41 rmr_dispatch_has_simd(RMR_SIMD_SSE41, RMR_COMPILETIME_HAS_SSE41)
#define RMR_HAS_AVX2 rmr_dispatch_has_simd(RMR_SIMD_AVX2, RMR_COMPILETIME_HAS_AVX2)
#define RMR_HAS_AVX512 rmr_dispatch_has_simd(RMR_SIMD_AVX512, RMR_COMPILETIME_HAS_AVX512)
#define RMR_HAS_NEON rmr_dispatch_has_simd(RMR_SIMD_NEON, RMR_COMPILETIME_HAS_NEON)

#if !defined(RMR_OS_WINDOWS) && !defined(RMR_OS_DARWIN) &&                \
    !defined(RMR_OS_LINUX) && !defined(RMR_OS_ANDROID) &&                 \
    !defined(RMR_OS_FREEBSD) && !defined(RMR_OS_OPENBSD) &&               \
    !defined(RMR_OS_NETBSD)
#define RMR_OS_UNKNOWN 1
#endif

#endif
