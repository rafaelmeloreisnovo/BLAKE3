// SPDX-License-Identifier: (MIT OR Apache-2.0 OR CC0-1.0)
// Minimal AArch64 ASM marker for the BLAKE3 NEON path.
//
// This file documents the ABI expectations for AArch64 (AAPCS64):
// - Arguments: x0-x7
// - Return: x0
// - Callee-saved: x19-x28, fp(x29), lr(x30), v8-v15
// - Stack alignment: 16 bytes at call boundaries
//
// The symbol below is intentionally a no-op marker so the build can
// include arch-specific ASM without changing the functional pipeline.

.text
.global blake3_neon_abi_marker
.type blake3_neon_abi_marker, %function
blake3_neon_abi_marker:
  ret

.size blake3_neon_abi_marker, .-blake3_neon_abi_marker
