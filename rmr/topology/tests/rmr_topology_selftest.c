/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under ../../LICENSE_RMR.
 */

#include "../include/rmr_topology.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

int main(void) {
  rmr_build_topology build = rmr_topology_compile_snapshot();
  rmr_runtime_counters counters;
  int value = 7;
  rmr_pointer_topology ptr = rmr_topology_pointer(&value, _Alignof(int));
  rmr_pointer_topology null_ptr = rmr_topology_pointer((const void *)0, _Alignof(int));

  if (build.surface_mask != rmr_topology_surface_mask()) {
    return 1;
  }
  if (build.pointer_bits != (uint16_t)(sizeof(void *) * (size_t)CHAR_BIT)) {
    return 2;
  }
  if (ptr.is_null != 0u || ptr.is_aligned == 0u) {
    return 3;
  }
  if (null_ptr.is_null == 0u || null_ptr.is_aligned != 0u) {
    return 4;
  }
  if (!rmr_topology_masks_overlap(0x12u, 0x02u)) {
    return 5;
  }
  if (rmr_topology_masks_overlap(0x10u, 0x02u)) {
    return 6;
  }

  rmr_topology_counter_reset(&counters);
  rmr_topology_counter_note_loop(&counters, 42u);
  rmr_topology_counter_note_io(&counters, 3u, 2u, 768u, 512u);

  if (counters.loop_iterations != 42u) {
    return 7;
  }
  if (rmr_topology_logical_io_ops(&counters) != 5u) {
    return 8;
  }
  if (counters.bytes_read != 768u || counters.bytes_written != 512u) {
    return 9;
  }

  return 0;
}
