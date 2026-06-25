# RAFAELIA Blacktrain — normative execution audit

Status: technical audit document. This is not a certification claim. Six Sigma, ISO, RFC, W3C, IEEE, BIST/FIPS-like and ICT concepts are used here as audit lenses to examine logic, structure, execution and binary path without changing the BLAKE3 cryptographic core.

## 1. Scope

Blacktrain means the current BLAKE3/RMR execution train as it exists in this repository:

```text
source code
-> build profile
-> compiler flags
-> binary layout
-> input buffering
-> BLAKE3 update path
-> SIMD / backend route
-> CV stack / lazy merge
-> finalize
-> benchmark / manifest / evidence
```

The audit boundary is strict:

```text
BLAKE3 core: preserve logic and output compatibility
RMR layer: observe, route, build, validate, measure, document
Binary path: examine size, symbols, sections, hot path, friction and evidence
```

## 2. Certification boundary

This repository is not claimed to be certified against ISO 27001, ISO 9001, ISO 8000, IEEE, W3C, FIPS, BIST, RFC or any other external standard.

The correct claim is:

> Blacktrain uses normative audit lenses to organize evidence, requirements, measurements and control gates. Certification requires an independent formal process and is outside this document.

## 3. Six Sigma DMAIC model

### Define

Problem statement:

> Identify where the binary execution path loses continuity or creates measurable friction without changing BLAKE3 semantics.

Critical-to-quality outputs:

- BLAKE3 output compatibility.
- Deterministic build metadata.
- Lower friction per byte processed.
- No heap/IO/syscall/log in hot path.
- Explicit backend route.
- Measurable binary size and section layout.
- Reproducible benchmark and manifest evidence.

### Measure

Minimum measurements:

| Metric | Why it matters |
|---|---|
| `bytes_per_update` | Detects fragmented input feed. |
| `updates_per_MiB` | Shows caller overhead. |
| `ns_per_byte` | Main throughput/latency measure. |
| `backend_selected` | Confirms SIMD/fallback route. |
| `size`, `.text`, `.data`, `.bss` | Binary footprint. |
| exported symbol count | Dynamic/shared-object surface. |
| `branch-misses` where available | Branch friction. |
| `cache-misses` where available | Memory/cache friction. |
| compiler/linker/flags | Reproducibility. |
| artifact hashes | Chain of custody. |

### Analyze

Likely root causes:

| Root cause | Symptom | Expected control |
|---|---|---|
| Input chunks too small | high `updates_per_MiB` | buffer staging before `update` |
| Non-power-of-two feed | subtree shrink loop activates | buffer sizes aligned to 1 KiB/64 KiB/powers of two |
| Wrong backend | fallback while SIMD is available | backend manifest and detector audit |
| Heap in hot path | allocator cost / jitter | arena/fixed buffer/nomalloc profile |
| IO/log in hot path | syscall stalls | move IO to pre/post stage |
| Excess exported symbols | larger shared metadata | hidden visibility / symbol audit |
| Warning noise | CI/build gate friction | warnings treated as defects or explicitly justified |
| Thermal/device variance | benchmark drift | record temperature/throttle when possible |

### Improve

Allowed improvement areas:

- Buffer staging before BLAKE3 update.
- Build profile selection and flag governance.
- Backend detection and fallback documentation.
- Binary-size measurement and symbol trimming.
- Hot-path audit automation.
- Manifest and metric schema.

Forbidden improvements:

- Changing BLAKE3 output semantics.
- Modifying official vectors to fit local results.
- Claiming speed/size/security without measured artifact.
- Mixing RMR infrastructure into upstream core without boundary documentation.

### Control

Control plan:

1. Preserve core vectors and compatibility tests.
2. Run build profile audit.
3. Run hot-path static audit.
4. Run binary size matrix.
5. Run buffer/friction benchmark matrix.
6. Store manifest and hashes.
7. Classify claim status: `measured`, `structural`, `pending`, `TOKEN_VAZIO`.

## 4. ISO 9001 lens — quality process

Blacktrain must be treated as a process, not a one-off build.

Process map:

```text
requirement
-> controlled profile
-> reproducible build
-> binary artifact
-> test assertion
-> benchmark evidence
-> review
-> control record
```

Existing strengths:

- Build profile centralization in `rmr/build/profiles.mk`.
- Explicit profiles: `latency`, `throughput`, `deterministic`, `debug`.
- Exported `RMR_FINAL_CFLAGS` and `RMR_FINAL_LDFLAGS`.
- Boundary between upstream BLAKE3 and RMR layer.

Quality gaps:

- Need a canonical evidence table per build profile.
- Need acceptance criteria per profile.
- Need standard artifact naming.
- Need repeatability thresholds.

## 5. ISO/IEC 27001 lens — security and risk

CIA model applied to Blacktrain:

| CIA axis | Blacktrain meaning | Control |
|---|---|---|
| Confidentiality | no accidental leakage from buffers/logs | no IO/log in hot path; scoped manifests |
| Integrity | hash/core correctness and artifact chain | vectors, hashes, manifests, signatures |
| Availability | deterministic operation and fallback route | backend fallback, no-libc mode, build profiles |

Risk register:

| Risk | Impact | Control |
|---|---|---|
| Core/RMR boundary confusion | false authorship or invalid claims | provenance and docs |
| Wrong backend selection | degraded performance | detector/backend manifest |
| Heap in critical route | jitter/latency spikes | `nomalloc` and fixed buffers |
| Silent build flag drift | incomparable results | record effective flags |
| Untracked binary artifact | unverifiable claim | hash and manifest |
| Unsupported environment | misleading result | target triple and device metadata |

## 6. ISO 8000 lens — data quality

A benchmark or binary-size claim is valid only when its data is complete enough to be interpreted.

Required fields:

```text
commit_sha
branch
build_profile
compiler
compiler_version
linker
CFLAGS
LDFLAGS
target_triple
architecture
OS/runtime
libc_mode: host-libc | no-libc | freestanding-nomalloc
backend_selected
input_size
buffer_size
updates_count
bytes_per_update
ns_per_byte
binary_path
binary_sha256
size_text
size_data
size_bss
exported_symbols
artifact_hashes
thermal_or_TOKEN_VAZIO
cache_counters_or_TOKEN_VAZIO
```

Data-quality rules:

- Missing metric is `TOKEN_VAZIO`, not guessed.
- Measured values must include command used to produce them.
- Claims must cite manifest IDs or artifact hashes.
- Different build profiles must not be mixed in one statistical run.

## 7. RFC 2119 / RFC 8174 requirements

The following words are normative inside this document when capitalized.

### MUST

- Blacktrain MUST preserve BLAKE3 output compatibility.
- Blacktrain MUST NOT change cryptographic semantics to improve benchmark results.
- Every benchmark MUST record effective CFLAGS and LDFLAGS.
- Hot path code MUST NOT perform file IO, socket IO, logging or repeated heap allocation.
- Binary-size claims MUST include `size` and section/symbol evidence.
- Friction claims MUST include a measured metric such as `ns_per_byte`, `updates_per_MiB`, cache/branch counters or a documented proxy.
- Missing data MUST be marked `TOKEN_VAZIO`.

### SHOULD

- The feed buffer SHOULD be tested at 1 KiB, 4 KiB, 16 KiB, 64 KiB, 256 KiB and 1 MiB.
- Runs SHOULD separate host-libc from freestanding-nomalloc.
- Runs SHOULD record selected backend and detector route.
- Builds SHOULD use centralized profiles instead of ad-hoc flags.
- Review SHOULD include static grep/audit of hot-path forbidden calls.

### MAY

- Blacktrain MAY use TBB or other optional routes when explicitly enabled.
- Blacktrain MAY include hardware counters where supported.
- Blacktrain MAY record socket/queue/TTL/retry state as context, but these are not friction unless they create measured stall or reprocessing.

## 8. W3C QA / test assertion lens

Each claim must become a testable assertion.

| Claim | Test assertion | Evidence |
|---|---|---|
| Core unchanged | Official vectors still pass | test log |
| Binary smaller | `size` delta vs baseline | size report |
| Less friction | lower `ns_per_byte` or lower `updates_per_MiB` | benchmark matrix |
| Hot path clean | no forbidden calls in hot path | static audit |
| no-libc route works | freestanding build succeeds | build log |
| Arena is single-image | one arena definition | symbol/object audit |
| Backend route valid | selected backend recorded | manifest |
| Data quality sufficient | manifest has required fields | schema validation |

## 9. BIST / FIPS-like self-test lens

This is not a FIPS validation claim. The useful concept is built-in self-test discipline.

Recommended self-tests:

| Self-test | Trigger | Failure action |
|---|---|---|
| known-answer BLAKE3 vector | before benchmark | fail run |
| ABI sizeof/offsetof probe | per target | mark ABI unknown if absent |
| binary-size probe | per build | block size claim if absent |
| hot-path static audit | per PR | block hot-path claim if fail |
| backend detector check | per runtime | mark backend unknown if absent |
| manifest schema check | per run | block evidence claim if incomplete |
| artifact hash check | per run | block custody claim if missing |

## 10. IEEE / V&V lens

Traceability model:

```text
requirement
-> design element
-> source file / build profile
-> test assertion
-> measurement artifact
-> claim status
```

Example:

| Requirement | Design element | Evidence |
|---|---|---|
| No heap in hot path | `HOTPATH_CONTRACT.md` + static grep | audit log |
| Freestanding no-libc path | `rmr_lowlevel.h` + arena C file | build + symbol report |
| Stable build profile | `profiles.mk` | manifest flags |
| BLAKE3 unchanged | upstream core boundary | vectors/test log |

## 11. ICT / runtime lens

Network, socket, retry, TTL and queue state are not friction by default.

They are context unless they cause:

- stall;
- reprocessing;
- blocking in hot path;
- buffer starvation;
- cache churn;
- syscall inside critical section;
- failure to enter SIMD/wide path.

Correct framing:

```text
context event != friction
context event + measured stall = friction
planned gate + deterministic release = structure
```

## 12. Binary path map

```text
1. source boundary
   - upstream BLAKE3 core
   - RMR external layer

2. build selection
   - profile
   - flags
   - linker policy

3. binary formation
   - sections
   - symbols
   - relocations
   - visibility

4. runtime feed
   - buffer size
   - update call count
   - alignment

5. BLAKE3 internal route
   - chunk_state
   - subtree power-of-two
   - compress_subtree_wide
   - SIMD degree
   - CV stack
   - lazy merge
   - finalize

6. evidence
   - benchmark
   - size report
   - manifest
   - hashes
   - claim gate
```

## 13. Friction matrix

| Friction point | Structural answer | Measurement |
|---|---|---|
| fragmented input | buffer staging | updates/MiB |
| non-power-of-two feed | align feed sizes | subtree shrink count or proxy |
| wrong backend | detector and backend manifest | backend_selected |
| heap in loop | nomalloc/static arena | static audit + allocator count |
| IO in hot path | move to pre/post stage | static audit |
| branch rare path | prevalidate before loop | branch miss / code review |
| cache churn | align buffer and batch size | cache counters or proxy |
| symbol bloat | hidden/private visibility | nm/readelf |
| binary bloat | flags/sections/strip | size/objdump |

## 14. Claim gate

| Claim type | Allowed wording | Forbidden wording without evidence |
|---|---|---|
| Structural | “the code is structured to support...” | “it proves faster” |
| Measured | “in this run, profile X measured Y” | “always faster” |
| Security | “integrity controls exist” | “ISO/FIPS certified” |
| Binary | “section/symbol report shows...” | “binary is smaller” without report |
| Execution | “hot path contract forbids...” | “no friction exists” |

## 15. Current conclusion

Blacktrain is structurally coherent as an execution train around BLAKE3. It already contains the main architectural ingredients needed for serious audit:

- preserved upstream cryptographic core;
- external RMR layer;
- build-profile governance;
- no-libc/freestanding nomalloc route;
- hot-path contract;
- binary dimensional audit;
- binary-size and encoding audit;
- tools for ABI and binary-size measurement.

The current limiting factor is not architecture. The limiting factor is measurement completeness.

```text
architecture: strong
claim discipline: strong
binary/ABI evidence: partial
runtime friction evidence: pending
certification status: not certified
```

## 16. Next control artifact

Recommended next file/tool:

```text
tools/blacktrain_friction_matrix.sh
```

It should produce:

- `blacktrain_manifest.json`;
- `binary_size.txt`;
- `symbols.txt`;
- `sections.txt`;
- `bench_buffer_matrix.tsv`;
- `claim_status.json`.

Until that exists, runtime-friction claims remain `structural` or `pending`, not final empirical claims.
