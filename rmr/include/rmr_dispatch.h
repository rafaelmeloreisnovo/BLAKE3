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

// Compile-time feature flags derived from rmr_arch.h + compiler defines.

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
#define RMR_HAS_SSE2 1
#else
#define RMR_HAS_SSE2 0
#endif

#if RMR_HAS_X86 && defined(__SSE4_1__)
#define RMR_HAS_SSE41 1
#else
#define RMR_HAS_SSE41 0
#endif

#if RMR_HAS_X86 && defined(__AVX2__)
#define RMR_HAS_AVX2 1
#else
#define RMR_HAS_AVX2 0
#endif

#if RMR_HAS_X86 && defined(__AVX512F__)
#define RMR_HAS_AVX512 1
#else
#define RMR_HAS_AVX512 0
#endif

#if RMR_HAS_ARM && (defined(__ARM_NEON) || defined(__ARM_NEON__))
#define RMR_HAS_NEON 1
#else
#define RMR_HAS_NEON 0
#endif

#if !defined(RMR_OS_WINDOWS) && !defined(RMR_OS_DARWIN) &&                \
    !defined(RMR_OS_LINUX) && !defined(RMR_OS_ANDROID) &&                 \
    !defined(RMR_OS_FREEBSD) && !defined(RMR_OS_OPENBSD) &&               \
    !defined(RMR_OS_NETBSD)
#define RMR_OS_UNKNOWN 1
#endif

#endif
