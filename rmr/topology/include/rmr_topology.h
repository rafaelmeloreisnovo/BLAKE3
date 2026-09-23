/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under ../../LICENSE_RMR.
 *
 * RMR build/hardware topology contract.
 * This module does not modify or replace the BLAKE3 core.
 */
#ifndef RMR_TOPOLOGY_H
#define RMR_TOPOLOGY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum rmr_topology_surface_bit {
  RMR_TOPOLOGY_PREPROCESSOR = (1u << 0),
  RMR_TOPOLOGY_COMPILER     = (1u << 1),
  RMR_TOPOLOGY_LINKER       = (1u << 2),
  RMR_TOPOLOGY_BINARY       = (1u << 3),
  RMR_TOPOLOGY_SYMBOL       = (1u << 4),
  RMR_TOPOLOGY_POINTER      = (1u << 5),
  RMR_TOPOLOGY_LOOP         = (1u << 6),
  RMR_TOPOLOGY_IO           = (1u << 7),
  RMR_TOPOLOGY_COMMENT      = (1u << 8),
  RMR_TOPOLOGY_WARNING      = (1u << 9),
  RMR_TOPOLOGY_MODULE       = (1u << 10),
  RMR_TOPOLOGY_OVERLAP      = (1u << 11),
  RMR_TOPOLOGY_CONDITION    = (1u << 12),
  RMR_TOPOLOGY_COLOR        = (1u << 13),
  RMR_TOPOLOGY_IOPS         = (1u << 14)
};

enum rmr_topology_arch {
  RMR_TOPOLOGY_ARCH_UNKNOWN = 0,
  RMR_TOPOLOGY_ARCH_X86 = 1,
  RMR_TOPOLOGY_ARCH_AARCH64 = 2,
  RMR_TOPOLOGY_ARCH_ARM = 3,
  RMR_TOPOLOGY_ARCH_RISCV = 4,
  RMR_TOPOLOGY_ARCH_PPC = 5
};

enum rmr_topology_compiler {
  RMR_TOPOLOGY_COMPILER_UNKNOWN = 0,
  RMR_TOPOLOGY_COMPILER_CLANG = 1,
  RMR_TOPOLOGY_COMPILER_GCC = 2,
  RMR_TOPOLOGY_COMPILER_MSVC = 3
};

enum rmr_topology_endian {
  RMR_TOPOLOGY_ENDIAN_UNKNOWN = 0,
  RMR_TOPOLOGY_ENDIAN_LITTLE = 1,
  RMR_TOPOLOGY_ENDIAN_BIG = 2
};

typedef struct rmr_build_topology {
  uint32_t surface_mask;
  uint32_t stdc_version;
  uint16_t pointer_bits;
  uint16_t size_t_bits;
  uint16_t char_bits;
  uint8_t arch;
  uint8_t compiler;
  uint8_t endian;
  uint8_t reserved;
} rmr_build_topology;

typedef struct rmr_pointer_topology {
  uintptr_t address;
  size_t requested_alignment;
  uint16_t pointer_bits;
  uint8_t is_null;
  uint8_t is_aligned;
} rmr_pointer_topology;

typedef struct rmr_runtime_counters {
  uint64_t loop_iterations;
  uint64_t logical_reads;
  uint64_t logical_writes;
  uint64_t bytes_read;
  uint64_t bytes_written;
} rmr_runtime_counters;

/* Stable mask describing the audit surfaces implemented by this module. */
uint32_t rmr_topology_surface_mask(void);

/* Compile/preprocessor snapshot. No runtime CPU probing is performed here. */
rmr_build_topology rmr_topology_compile_snapshot(void);

/*
 * Pointer metadata only. The pointer is never dereferenced.
 * requested_alignment == 0 means alignment is unknown/not requested.
 */
rmr_pointer_topology rmr_topology_pointer(const void *ptr, size_t requested_alignment);

/* Bit-topology overlap primitive. */
int rmr_topology_masks_overlap(uint64_t lhs, uint64_t rhs);

/* Explicit logical counters; they do not claim physical storage IOPS. */
void rmr_topology_counter_reset(rmr_runtime_counters *counters);
void rmr_topology_counter_note_loop(rmr_runtime_counters *counters, uint64_t iterations);
void rmr_topology_counter_note_io(
    rmr_runtime_counters *counters,
    uint64_t reads,
    uint64_t writes,
    uint64_t bytes_read,
    uint64_t bytes_written);
uint64_t rmr_topology_logical_io_ops(const rmr_runtime_counters *counters);

#ifdef __cplusplus
}
#endif

#endif
