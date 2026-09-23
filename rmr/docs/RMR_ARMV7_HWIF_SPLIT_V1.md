<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR ARMv7 HWIF Split V1

State: SOURCE_IMPLEMENTED_PENDING_CI  
Claim allowed: false

## Boundary

ARMv7 user mode and privileged execution are different authority domains:

```text
ARMV7_USER != ARMV7_PRIVILEGED
NEON_AVAILABLE != CP15_ACCESS_ALLOWED
COMPILES != EXECUTES_ON_DEVICE
```

The user leaf `rmr/hwif/asm/armv7/rmr_hwif_user.S` performs only the Linux
ARM EABI write syscall. It does not read PMCCNTR, MIDR or MPIDR.

The privileged leaf `rmr/hwif/asm/armv7/rmr_hwif_privileged.S` is reachable
only when `RMR_ARMV7_ASSUME_PRIVILEGED=1` is selected. It exposes:

- PMCCNTR as a zero-extended 32-bit raw cycle counter;
- MIDR in the high 32 bits and MPIDR in the low 32 bits of the fingerprint.

The fingerprint is not a PUF or globally unique serial.

## Width correction

`rmr_u64` and `rmr_s64` use `uint64_t` and `int64_t`. This removes the
LP32 ambiguity where `unsigned long` is only 32 bits on ARMv7.

## Capabilities

Backends expose capability flags so backend identity cannot be confused with
the provenance of every operation:

- TIME_HARDWARE
- CPU_ID_HARDWARE
- RAW_WRITE_DIRECT
- PRIVILEGED_REGS

The ARMv7 user backend advertises direct write only. Time/ID remain software
fallbacks unless the privileged backend is explicitly selected.

## Cross matrix

`rmr/hwif/build/build_cross_matrix.sh` builds relocatable linked objects for:

1. x86_64;
2. AArch64;
3. ARMv7 Linux user;
4. ARMv7 privileged.

No unavailable linker is simulated or inferred.

The matrix proves cross-architecture compilation plus object-level symbol
closure. Native x86_64 linking is exercised by the CMake self-test. The current
CI runner does not expose a cross linker, therefore cross-executable linking is
`TOKEN_VAZIO_RUNNER_NO_LINKER`. Physical ARMv7 execution remains TOKEN_VAZIO
until a device receipt exists.
