<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under ../LICENSE_RMR.
-->

# RMR Standalone Foundation V1

Small authorial foundation for new RMR pure-core modules.

## Properties

- no libc in the production core;
- no heap;
- no syscall;
- no filesystem;
- no clock;
- compiler-native integer/size types;
- caller-owned buffers;
- explicit status values;
- Q8/Q16/Q32/Q42 representation profiles;
- strict warning profile;
- section GC + safe ICF in the LLD link proof;
- explicit entrypoint;
- cross-link targets: x86_64, AArch64, ARMv7.

This module does **not** replace BLAKE3 and does not implement cryptography.

## Build

Hosted semantic selftest:

```sh
sh rmr/standalone/build/build_host_selftest.sh
```

Freestanding cross-link proof:

```sh
sh rmr/standalone/build/build_cross_matrix.sh
```

## Binary gate

Each produced ELF must have:

```text
PT_INTERP = absent
DT_NEEDED = absent
unexpected UND = 0
forbidden runtime symbols = 0
```

The build script itself runs on a host and requires compiler/binutils. That is a **build-time tool dependency**, not a runtime dependency of the resulting pure-core artifact.

## Comment policy

Comments describe contracts and rationale. They are not relied on to reduce binary size.

Actual binary-shaping mechanisms are compiler/linker flags, attributes, visibility, sections, entrypoints and code structure.
