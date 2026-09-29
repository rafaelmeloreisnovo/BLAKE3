# RMR Portable V1

State: IMPLEMENTED_UNTESTED

RMR Portable V1 is a new, prospective, project-authored portability layer. It
does not modify or rename upstream BLAKE3.

## Surfaces

```text
PURE_FIXED_CORE
  C fixed 256-byte reducer
  zero heap / libc / syscall / filesystem / clock / network
  fixed frame -> no variable tail path

BLAKE3_PROVIDER
  in-memory adapter only
  upstream BLAKE3 remains upstream
  optional freestanding link uses project memory shims
  no file I/O in provider adapter

RUST
  #![no_std] direct C ABI bridge
  no allocator / crate dependency

JAVA
  JDK-language-only fixed-frame mirror
  no imports / I/O / allocation inside kernel
  JVM/Android runtime remains an external platform

BATCH4
  four fixed frames
  statically unrolled calls
  no dynamic scheduler
```

## Measurable low-level gates

```text
heap_calls = 0
syscalls_in_pure_core = 0
filesystem_calls_in_pure_core = 0
network_calls_in_pure_core = 0
clock_calls_in_pure_core = 0
unexpected_undefined_symbols = 0
DT_NEEDED = 0
PT_INTERP = 0
-Wshadow -Werror
variable_tail_path_fixed256 = 0
runtime_dynamic_dispatch_fixed256 = 0
```

The project does not claim that compilable C/Rust/Java can literally contain
no identifiers, no functions, or no control flow. Instead it minimizes public
symbols and proves dependency closure at the binary boundary.

## Architecture

```text
caller-owned bytes
        |
        +--> RMR authorial fixed-frame core
        |
        +--> BLAKE3 provider adapter --> upstream BLAKE3
        |
        +--> Rust no_std ABI
        |
        +--> Java fixed-frame mirror
        |
        +--> batch4 direct surface
```

OS, filesystem, Android UI, sockets, clocks, and process launch belong in
separate adapters. They never become properties of the pure core.

## License

Only new files explicitly carrying
`LicenseRef-RMR-Individual-Research-1.0` are covered by
`LICENSE_RMR_INDIVIDUAL_RESEARCH_V1.txt`.

Older RMR files retain their prior grant. Upstream BLAKE3 retains upstream
licenses.
