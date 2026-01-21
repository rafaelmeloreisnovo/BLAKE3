// SPDX-License-Identifier: (MIT OR Apache-2.0 OR CC0-1.0)
// Minimal ARMv7 ASM marker for the BLAKE3 NEON path.
//
// ABI expectations for ARMv7 AAPCS:
// - Arguments: r0-r3
// - Return: r0
// - Callee-saved: r4-r11, sp, lr
// - Stack alignment: 8 bytes at call boundaries (AAPCS)
//
// The symbol below is intentionally a no-op marker so the build can
// include arch-specific ASM without changing the functional pipeline.

.text
.global blake3_neon_abi_marker
.type blake3_neon_abi_marker, %function
blake3_neon_abi_marker:
  bx lr

.size blake3_neon_abi_marker, .-blake3_neon_abi_marker
