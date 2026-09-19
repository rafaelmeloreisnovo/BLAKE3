# RMR FIXED256 evidence

Verification date: 2026-09-18.

Local toolchain used for this verification:

- Clang 17.0.0
- LLD 17.0.0
- javac 21.0.11

Executed:

```text
[OK] x86_64
[OK] aarch64
[OK] armv7
RMR_FIXED256_JAVA_OK
```

`readelf` inspection of the three linked ASM probes reported:

- x86_64: ELF64, machine x86-64;
- AArch64: ELF64, machine AArch64;
- ARMv7: ELF32, machine ARM;
- no `PT_INTERP`;
- no `DT_NEEDED`;
- no undefined symbol other than the conventional null symbol-table entry.

This evidence proves source assembly/linkability under the recorded toolchain and the Java self-test. It does not prove hardware performance, constant-time cryptography, JIT behavior, or BLAKE3 equivalence.
