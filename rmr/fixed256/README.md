# RMR FIXED256 — bare-metal ASM + Java low-level twin

Two implementations of the same fixed-frame reducer:

- `asm/`: x86_64/SSE2, AArch64/ASIMD and ARMv7/NEON; standalone `_start`; no libc/runtime.
- `java/`: JDK-only source with no imports or allocation inside the kernel.

The fixed frame is 256 bytes (16 × 16). The result is a 16-byte XOR reduction across lanes. Fixed framing deliberately removes a variable-length tail path.

## Verify

```sh
sh rmr/fixed256/build/build_asm_probes.sh
sh rmr/fixed256/build/run_java_selftest.sh
```

The ASM build rejects `DT_NEEDED`, `PT_INTERP` and unexpected undefined symbols when `readelf` is available.

This module is a deterministic low-level transport/custody primitive, **not cryptography** and **not BLAKE3**.
