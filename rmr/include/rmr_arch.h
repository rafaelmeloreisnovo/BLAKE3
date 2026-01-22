/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */
#ifndef RMR_ARCH_H
#define RMR_ARCH_H

// Compiler detection.
#if defined(__clang__)
#define RMR_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define RMR_COMPILER_GCC 1
#elif defined(_MSC_VER)
#define RMR_COMPILER_MSVC 1
#endif

// Operating system detection.
#if defined(_WIN32) || defined(_WIN64)
#define RMR_OS_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#define RMR_OS_DARWIN 1
#elif defined(__linux__)
#define RMR_OS_LINUX 1
#elif defined(__ANDROID__)
#define RMR_OS_ANDROID 1
#elif defined(__FreeBSD__)
#define RMR_OS_FREEBSD 1
#elif defined(__OpenBSD__)
#define RMR_OS_OPENBSD 1
#elif defined(__NetBSD__)
#define RMR_OS_NETBSD 1
#endif

// Architecture detection.
#if defined(__x86_64__) || defined(_M_X64)
#define RMR_ARCH_X86_64 1
#elif defined(__i386__) || defined(_M_IX86)
#define RMR_ARCH_X86_32 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define RMR_ARCH_AARCH64 1
#elif defined(__arm__) || defined(_M_ARM)
#define RMR_ARCH_ARM 1
#elif defined(__powerpc64__) || defined(__ppc64__)
#define RMR_ARCH_PPC64 1
#elif defined(__powerpc__) || defined(__ppc__)
#define RMR_ARCH_PPC32 1
#elif defined(__riscv) || defined(__riscv__)
#define RMR_ARCH_RISCV 1
#endif

// Endianness detection.
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define RMR_LITTLE_ENDIAN 1
#else
#define RMR_BIG_ENDIAN 1
#endif
#elif defined(_WIN32) || defined(__LITTLE_ENDIAN__)
#define RMR_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__)
#define RMR_BIG_ENDIAN 1
#endif

// Assembly family hints.
#if defined(RMR_ARCH_X86_64) || defined(RMR_ARCH_X86_32)
#define RMR_ASM_INTEL 1
#elif defined(RMR_ARCH_AARCH64) || defined(RMR_ARCH_ARM)
#define RMR_ASM_ARM 1
#endif

#endif
