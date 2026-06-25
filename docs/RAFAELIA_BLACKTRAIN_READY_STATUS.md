# RAFAELIA Blacktrain — ready status summary

Status: operational summary based on the current repository structure, recent merged PRs, and the open dimensional/normative audit branch. This document separates what is ready, what is structurally present, what is only partially measured, and what remains `TOKEN_VAZIO`.

## 1. Executive summary

Blacktrain is not just a BLAKE3 fork. It is an execution train around BLAKE3 with an external RMR layer for build governance, runtime boundary, no-libc/nomalloc support, hot-path discipline, benchmark/custody tooling and binary-dimensional documentation.

The strongest current statement is:

> The BLAKE3 cryptographic core remains preserved, while the RMR/Blacktrain layer organizes build profiles, native/freestanding paths, hot-path rules, binary size/encoding analysis and measurement tooling around it.

The current weak point is not structure. The weak point is full runtime measurement on target devices, especially ARMv7/Termux/Android.

## 2. What is ready

| Area | Status | Evidence/meaning |
|---|---:|---|
| Upstream/core boundary | Ready | Core BLAKE3 folders are treated as preserved upstream domains. |
| RMR external layer | Ready | Authorial logic is isolated under `rmr/` and docs. |
| Build profile governance | Ready | `rmr/build/profiles.mk` centralizes `latency`, `throughput`, `deterministic`, `debug`. |
| Effective flags model | Ready | `RMR_FINAL_CFLAGS` and `RMR_FINAL_LDFLAGS` are exported by profile. |
| Freestanding/no-libc route | Ready structurally | `RMR_NO_LIBC` and `RMR_FREESTANDING_NOMALLOC` provide an arena path. |
| Single arena per image | Ready structurally | Arena state lives in `rmr/core/lowlevel_freestanding.c`. |
| Hot-path contract | Ready | `rmr/docs/HOTPATH_CONTRACT.md` defines allowed/prohibited critical-route behavior. |
| Runtime detector boundary | Ready structurally | `rmr/hwif/detect/*` and `rmr/hwif/include/rmr_detect.h` define detection boundaries. |
| Pathcutter isolation | Ready | Host/POSIX support helpers are isolated in `rmr/pathcutter/`. |
| Benchmark/custody direction | Ready structurally | Bench/benchdiff/manifest/hashchain concepts exist in RMR tooling. |
| Dimensional BLAKE3 audit | Ready in PR #98 | Documents 32/64/1024/depth/CV stack/SIMD geometry. |
| Binary-size/encoding audit | Ready in PR #98 | Documents flags, hexadecimal, symbols, visibility and binary-size gates. |
| ABI size probe | Ready in PR #98 | `tools/sizeof_blake3_state.c` measures ABI-dependent struct sizes. |
| Binary-size probe | Ready in PR #98 | `tools/binary_size_matrix.sh` produces size/section/symbol reports. |
| Normative execution audit | Ready in PR #98 | `RAFAELIA_BLACKTRAIN_NORMATIVE_EXECUTION_AUDIT.md`. |

## 3. What is structurally strong

### 3.1 Core preservation

The repository uses a clean boundary:

```text
BLAKE3 core: src/, c/, reference_impl/, b3sum/, test_vectors/
RMR/Blacktrain: rmr/, docs, tools, benchmark/custody support
```

Meaning:

- BLAKE3 result compatibility remains the controlling constraint.
- RMR can innovate around execution, build, custody and measurement without claiming authorship over the BLAKE3 primitive.

### 3.2 Build profiles

The build profile layer is strong because it turns flags into policy:

```text
latency       -> -O2
throughput    -> -O3
deterministic -> -O2 -fno-builtin
debug         -> -O0 -g3
base flags    -> -ffreestanding -fno-stack-protector
base link     -> -nostdlib -Wl,-e,_start -pie
```

This is not random optimization. It is controlled build governance.

### 3.3 Host-libc vs no-libc boundary

Blacktrain does not break libc.

It defines two routes:

```text
host route:
  rmr_ll_malloc -> malloc
  rmr_ll_free   -> free

freestanding route:
  rmr_ll_malloc -> static bump arena
  rmr_ll_free   -> no-op
  reset         -> rmr_ll_freestanding_reset_allocator
```

This is important because it allows both normal host behavior and constrained freestanding operation without rewriting the BLAKE3 core.

### 3.4 Hot path discipline

The hot-path contract is strong because it forbids:

- IO in hot path;
- repeated heap allocation in critical loops;
- `call`/`bl` in backend ASM hot path;
- logging/OS access in dispatcher selection;
- lock/mutex insertion in backend dispatcher.

This matches the Blacktrain idea:

```text
prepare -> gate -> release -> execute -> merge -> next stage
```

not:

```text
execute -> stall -> log -> retry -> malloc -> return to beginning
```

## 4. What has been documented in the current audit branch

| Document/tool | Purpose |
|---|---|
| `docs/RAFAELIA_BLAKE3_DIMENSIONAL_AUDIT.md` | Binary-dimensional reading of BLAKE3: 32-byte CVs, 64-byte blocks, 1024-byte chunks, depth 54, stack and SIMD degree. |
| `tools/sizeof_blake3_state.c` | Measures real ABI-dependent struct sizes and offsets. |
| `docs/RAFAELIA_BLAKE3_BINARY_SIZE_AND_ENCODING_AUDIT.md` | Explains binary encoding discipline: hex constants, bit flags, loops, visibility, no RTTI/exceptions. |
| `tools/binary_size_matrix.sh` | Measures size, sections, symbols, stripped/unstripped outputs for binary-size evidence. |
| `docs/RAFAELIA_BLACKTRAIN_NORMATIVE_EXECUTION_AUDIT.md` | Normative audit using Six Sigma, ISO, RFC, W3C, BIST and V&V lenses. |
| `docs/RAFAELIA_BLACKTRAIN_READY_STATUS.md` | This summary of what is ready, partial and pending. |

## 5. What is partially ready

| Area | Partial reason | Required next evidence |
|---|---|---|
| Binary smaller claim | Structural reasons exist, but size report must be produced. | `size`, `objdump -h`, `nm`, `readelf`, stripped size. |
| Less friction claim | Hot-path contract exists, but runtime counters are missing. | `ns/byte`, `updates/MiB`, backend, buffer matrix. |
| ARMv7/Termux readiness | Target intent is clear, but target run not recorded here. | Run ABI/binary probes on device. |
| Cache/buffer claim | BLAKE3 geometry supports it, but cache counters/proxy needed. | `perf`, `simpleperf`, or buffer-size benchmark proxy. |
| Backend route | Detector boundary exists, but each run must record selected backend. | Manifest field `backend_selected`. |
| no-libc behavior | Arena path exists, but binary proof needed per target. | freestanding build log + symbol report. |
| Data quality | Manifest direction exists, but schema should be canonical. | `blacktrain_manifest.schema.json`. |

## 6. What remains TOKEN_VAZIO

`TOKEN_VAZIO` means do not invent evidence.

| Claim/data | Current status |
|---|---|
| Exact ARMv7 `sizeof(blake3_hasher)` | TOKEN_VAZIO until target probe runs. |
| Exact Android/Termux binary size | TOKEN_VAZIO until built and measured. |
| Exact cache-miss reduction | TOKEN_VAZIO until measured or proxied. |
| Exact branch-miss behavior | TOKEN_VAZIO until measured or inspected at assembly level. |
| Exact performance gain vs upstream | TOKEN_VAZIO until benchmark matrix is complete. |
| ISO/FIPS/IEEE certification | TOKEN_VAZIO / not claimed. |
| Socket/TTL/retry friction | TOKEN_VAZIO unless measured as stall/reprocessing. |

## 7. Current maturity score by layer

This is an internal engineering maturity estimate, not an external certification score.

| Layer | Maturity | Reason |
|---|---:|---|
| Conceptual architecture | 85% | Clear event/gate/stage model. |
| Core preservation | 90% | Upstream/RMR boundary is explicit. |
| Build governance | 80% | Profiles and flags are centralized. |
| no-libc/freestanding structure | 75% | Arena exists; target proof pending. |
| Hot path rules | 80% | Contract exists; static audit should be regularized. |
| Binary dimensional audit | 75% | Strong structural documentation; ABI target runs pending. |
| Binary size evidence | 45% | Tool exists; measured reports pending. |
| Runtime friction evidence | 35% | Method defined; buffer/backend matrix pending. |
| Data-quality schema | 45% | Fields known; canonical schema pending. |
| Certification readiness | 20% | Audit lenses exist; no certification process claimed. |

## 8. Ready-to-say claims

The following claims are currently defensible:

1. The project preserves the BLAKE3 cryptographic core as the compatibility boundary.
2. The RMR/Blacktrain layer is external and authorial.
3. Build profiles and effective flags are centralized.
4. A no-libc/freestanding nomalloc route exists structurally.
5. Hot-path rules are documented and prohibit common friction sources.
6. BLAKE3 dimensional geometry is documented: 32-byte CV/hash, 64-byte block, 1024-byte chunk, depth 54.
7. Binary-size and encoding issues are documented as structural claims.
8. Measurement tools have been added for ABI and binary-size evidence.
9. Runtime-friction claims are not final until measured.

## 9. Not-yet-ready claims

Do not claim these yet:

1. “Blacktrain is certified ISO/FIPS/IEEE.”
2. “The binary is always smaller.”
3. “Execution is always faster.”
4. “Cache behavior is proven better.”
5. “ARMv7/Termux behavior is proven.”
6. “No friction exists.”
7. “Socket/queue/retry/TTL are irrelevant in all cases.”
8. “RMR changes the BLAKE3 algorithm.”

Correct replacement wording:

> Blacktrain has structural controls intended to reduce binary/runtime friction. Measurement artifacts are required before final performance or size claims.

## 10. Next execution checklist

Run on the actual target when possible:

```sh
# 1. ABI/layout proof
cc -std=c11 -Ic tools/sizeof_blake3_state.c -o sizeof_blake3_state
./sizeof_blake3_state

# 2. Binary-size proof
sh tools/binary_size_matrix.sh

# 3. Hot-path static triage
rg -n "printf|fprintf|fopen|write\(|send\(|recv\(|socket\(" rmr/hwif rmr/core
rg -n "malloc\(|calloc\(|realloc\(|free\(" rmr/core
rg -n "\b(call|bl)\b" rmr/hwif/asm

# 4. Buffer/friction benchmark matrix
# pending tool: tools/blacktrain_friction_matrix.sh
```

## 11. Final current state

```text
What is ready:
  architecture, boundaries, build profiles, hot-path contract, no-libc route, dimensional docs, binary-size docs, ABI/binary probes.

What is almost ready:
  formal claim gates and measurement schema.

What still needs execution:
  real target ABI report, binary-size report, runtime buffer/friction matrix, backend manifest, cache/branch/cycle evidence.
```

## 12. Short summary

Blacktrain is ready as an auditable execution architecture. It is not yet complete as a measured performance claim.

The correct stage is:

```text
structure: ready
instrumentation: partially ready
empirical proof: pending
certification: not claimed
```
