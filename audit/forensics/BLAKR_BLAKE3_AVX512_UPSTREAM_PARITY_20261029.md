# BLAKE3 upstream AVX-512 soundness fix: parity gap at pinned RMR HEAD — 2026-10-29

**State:** VERIFIED_SOURCE_PARITY_GAP / RUNTIME_IMPACT_UNTESTED / DRAFT_HOLD / claim_allowed=false

## Exact source boundaries
- Official upstream reference: `BLAKE3-team/BLAKE3@f55849f89cd85083c1c9daa2c5de20766f309c00`, [upstream commit, 2026-10-01](https://github.com/BLAKE3-team/BLAKE3/commit/f55849f89cd85083c1c9daa2c5de20766f309c00).
- User fork reference: `rafaelmeloreisnovo/BLAKE3@718a8f8e200c42aecfdf5ffedcab6d130ed3dcfb`, [fork HEAD snapshot, 2026-10-27](https://github.com/rafaelmeloreisnovo/BLAKE3/commit/718a8f8e200c42aecfdf5ffedcab6d130ed3dcfb).
- Git compare `BLAKE3-team:master...master` as observed 2026-10-29: ahead 1060, behind 29, merge-base `890050cb9b89448a25e067349aed9734a3b03d5e`. That comparison is broad; this document is scoped to a *single* upstream patch.

## Source-level observations: READBACK_VERIFIED
| File | Upstream blob SHA | Fork blob SHA | Specific comparison |
|---|---|---|---|
| `c/blake3_avx512_x86-64_unix.S` | `ae15eaed7ab0531c2935177646d917987aaa7fad` | `9642e413a3ad0f959449e0089dfd672fbff9fb86` | upstream inserts `jb 4f` after `cmp r10,0x1` / `ja 2f`; fork lacks branch at same function entry |
| `src/test.rs` | `26672f4268c3302dff013e25ca697060fbcc88a8` | `9bb6c4a716044e4cbb0fdab5687a3f6f941c7e3b` | `test_xof_many_rounds_down` exists upstream, absent in fork snapshot |

The upstream patch also introduces the `4:` return label; its own commit description says a safe hidden-but-public benchmark-facing `platform.xof_many` path with output buffer shorter than a 64-byte block could previously write out of bounds in AVX-512 release builds. The upstream authors explicitly state the **documented public `blake3` API is unaffected**.

## Interpretation / falsifiers
- **Confirmed (source):** exact fork snapshot does **not** contain the corresponding AVX-512 short-circuit instruction or regression test. This is a parity gap relative to upstream fix `f55849f`.
- **NOT confirmed (runtime):** whether the fork exposes or reaches that path in any production build; compiler/architecture availability; whether equivalent mitigations exist elsewhere; practical OOB on the user's binaries; any impact on users. No exploit or executable PoC was run.
- **NOT inferred:** malicious omission, Wayback alteration, shared IIS, historical 1999–2017 website activity, or cryptographic break.

## Recommended independent verification (HOLD)
1. Create a **separate source-fix proposal** in the producer fork, not in this documentation Draft, after security-maintainer review of AGENTS and upstream license. Do not paste arbitrary changes to core without gate.
2. Port only the upstream 2026-10-01 patch (assembly return guard and upstream regression test), preserving authorship/attribution; use a precise diff with current HEAD lease.
3. Validate dedicated host builds with real AVX-512 gating and Rust test suite, sanitizers where supported, short output length behavior, canonical hash vectors and reproducible CI at the tested exact HEAD.
4. Confirm code review, required checks, branch protection, CI on independent runner, rollback and notification obligations before merging any fix.
5. Keep documentation-only PR as **DRAFT/HOLD**; it must never silently mutate the cryptographic source.

### Audit receipt
`MU-BLAKR-BLAKE3-AVX512-UPSTREAM-PARITY-20261029-V1 | kind=SOURCE_DIFF_VERIFIED | source=upstream f55849f + both pinned GitHub blobs | evidence=paired blob SHA + presence/absence of branch and regression test | gap=runtime test, security maintainer review, release exposure | next=scoped port+test in separate approved PR | claim_allowed=false | no merge`.

Crosswalk: [LGPD historical-site audit PR#35](https://github.com/instituto-Rafael/LGPD-Constituicoes-planetaria-paises-onu-direitos-humanos-e-fundamentais-de-cada-continents-geologic/pull/35). This code issue is temporally and causally **independent** of unresolved 1999–2017 Wayback/IIS hypotheses.
