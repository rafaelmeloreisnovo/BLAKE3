# Release gate — v0.2.0-rc.1

- Version: `0.2.0-rc.1` applies ONLY to `rmr/prospective_release_v020` — NOT the upstream BLAKE3 crate, C provider, or entire historical RMR.
- Source baseline: GitHub `rafaelmeloreisnovo/BLAKE3`, `master@00fd8bd010ee598993f6c6d9d9969ce5e309e906`.
- Version delta: new versioned fixed-schema 64-byte release metadata transport; new C11 freestanding unit and independent CRC32C KAT, schema/zero digest/corruption negative tests; does not change BLAKE3 algorithm or claim cryptographic originality.
- Licenses: `rmr/LICENSE_RMR` and upstream terms unchanged; `LICENSE_CANDIDATE.md` is prospective and NOT ACTIVE. No retroactive relicensing or automatic commercial restriction for old code.
- Source host gcc/clang pass: TOKEN_VAZIO until exact branch test observed.
- Cross object ARMv7/ARM64: TOKEN_VAZIO until toolchain test.
- CI exact-head: TOKEN_VAZIO until run.
- Device/ABI/apk/performance: TOKEN_VAZIO_NOT_RUN.
- Legal counsel and chain of title: TOKEN_VAZIO.
- Commercial agreement/signed parties/licensor authorization: TOKEN_VAZIO.
- Final release/publish/tag: BLOCKED_HOLD.
- Human independent review/provider protection: TOKEN_VAZIO.
- Definition of acceptance: review signed legal title + verified exact-SHA compile/KAT + negative tests + external review; scope claim remains limited to new module.
