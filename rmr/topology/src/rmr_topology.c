/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under ../../LICENSE_RMR.
 */

#include "../include/rmr_topology.h"
#include "../../include/rmr_arch.h"

#include <limits.h>
#include <stdint.h>

static uint64_t rmr_sat_add_u64(uint64_t lhs, uint64_t rhs) {
  if (UINT64_MAX - lhs < rhs) {
    return UINT64_MAX;
  }
  return lhs + rhs;
}

uint32_t rmr_topology_surface_mask(void) {
  return
      RMR_TOPOLOGY_PREPROCESSOR |
      RMR_TOPOLOGY_COMPILER |
      RMR_TOPOLOGY_LINKER |
      RMR_TOPOLOGY_BINARY |
      RMR_TOPOLOGY_SYMBOL |
      RMR_TOPOLOGY_POINTER |
      RMR_TOPOLOGY_LOOP |
      RMR_TOPOLOGY_IO |
      RMR_TOPOLOGY_COMMENT |
      RMR_TOPOLOGY_WARNING |
      RMR_TOPOLOGY_MODULE |
      RMR_TOPOLOGY_OVERLAP |
      RMR_TOPOLOGY_CONDITION |
      RMR_TOPOLOGY_COLOR |
      RMR_TOPOLOGY_IOPS;
}

rmr_build_topology rmr_topology_compile_snapshot(void) {
  rmr_build_topology out;

  out.surface_mask = rmr_topology_surface_mask();
#if defined(__STDC_VERSION__)
  out.stdc_version = (uint32_t)__STDC_VERSION__;
#else
  out.stdc_version = 0u;
#endif
  out.pointer_bits = (uint16_t)(sizeof(void *) * (size_t)CHAR_BIT);
  out.size_t_bits = (uint16_t)(sizeof(size_t) * (size_t)CHAR_BIT);
  out.char_bits = (uint16_t)CHAR_BIT;
  out.arch = RMR_TOPOLOGY_ARCH_UNKNOWN;
  out.compiler = RMR_TOPOLOGY_COMPILER_UNKNOWN;
  out.endian = RMR_TOPOLOGY_ENDIAN_UNKNOWN;
  out.reserved = 0u;

#if defined(RMR_ARCH_X86_64) || defined(RMR_ARCH_X86_32)
  out.arch = RMR_TOPOLOGY_ARCH_X86;
#elif defined(RMR_ARCH_AARCH64)
  out.arch = RMR_TOPOLOGY_ARCH_AARCH64;
#elif defined(RMR_ARCH_ARM)
  out.arch = RMR_TOPOLOGY_ARCH_ARM;
#elif defined(RMR_ARCH_RISCV)
  out.arch = RMR_TOPOLOGY_ARCH_RISCV;
#elif defined(RMR_ARCH_PPC64) || defined(RMR_ARCH_PPC32)
  out.arch = RMR_TOPOLOGY_ARCH_PPC;
#endif

#if defined(RMR_COMPILER_CLANG)
  out.compiler = RMR_TOPOLOGY_COMPILER_CLANG;
#elif defined(RMR_COMPILER_GCC)
  out.compiler = RMR_TOPOLOGY_COMPILER_GCC;
#elif defined(RMR_COMPILER_MSVC)
  out.compiler = RMR_TOPOLOGY_COMPILER_MSVC;
#endif

#if defined(RMR_LITTLE_ENDIAN)
  out.endian = RMR_TOPOLOGY_ENDIAN_LITTLE;
#elif defined(RMR_BIG_ENDIAN)
  out.endian = RMR_TOPOLOGY_ENDIAN_BIG;
#endif

  return out;
}

rmr_pointer_topology rmr_topology_pointer(const void *ptr, size_t requested_alignment) {
  rmr_pointer_topology out;
  uintptr_t address = (uintptr_t)ptr;

  out.address = address;
  out.requested_alignment = requested_alignment;
  out.pointer_bits = (uint16_t)(sizeof(void *) * (size_t)CHAR_BIT);
  out.is_null = (uint8_t)(ptr == (const void *)0);
  out.is_aligned = 0u;

  if (ptr != (const void *)0 && requested_alignment != 0u) {
    out.is_aligned = (uint8_t)((address % (uintptr_t)requested_alignment) == 0u);
  }

  return out;
}

int rmr_topology_masks_overlap(uint64_t lhs, uint64_t rhs) {
  return (lhs & rhs) != 0u;
}

void rmr_topology_counter_reset(rmr_runtime_counters *counters) {
  if (counters == (rmr_runtime_counters *)0) {
    return;
  }

  counters->loop_iterations = 0u;
  counters->logical_reads = 0u;
  counters->logical_writes = 0u;
  counters->bytes_read = 0u;
  counters->bytes_written = 0u;
}

void rmr_topology_counter_note_loop(rmr_runtime_counters *counters, uint64_t iterations) {
  if (counters == (rmr_runtime_counters *)0) {
    return;
  }
  counters->loop_iterations = rmr_sat_add_u64(counters->loop_iterations, iterations);
}

void rmr_topology_counter_note_io(
    rmr_runtime_counters *counters,
    uint64_t reads,
    uint64_t writes,
    uint64_t bytes_read,
    uint64_t bytes_written) {
  if (counters == (rmr_runtime_counters *)0) {
    return;
  }

  counters->logical_reads = rmr_sat_add_u64(counters->logical_reads, reads);
  counters->logical_writes = rmr_sat_add_u64(counters->logical_writes, writes);
  counters->bytes_read = rmr_sat_add_u64(counters->bytes_read, bytes_read);
  counters->bytes_written = rmr_sat_add_u64(counters->bytes_written, bytes_written);
}

uint64_t rmr_topology_logical_io_ops(const rmr_runtime_counters *counters) {
  if (counters == (const rmr_runtime_counters *)0) {
    return 0u;
  }
  return rmr_sat_add_u64(counters->logical_reads, counters->logical_writes);
}
