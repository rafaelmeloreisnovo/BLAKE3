# μ0002 — Upstream CI provenance

- date: 2026-09-27
- clock: TOKEN_VAZIO
- parent_commit: 7c3c9c87f4b1c0d360a3c4bd97636d7926f48b25
- kind: PROVENANCE
- source_path: .github/workflows/ci.yml
- claim_allowed: false

## Observed provenance

The current fork `.github/workflows/ci.yml` and current `BLAKE3-team/BLAKE3` `.github/workflows/ci.yml` resolve to the same blob SHA:

`f7dd8c053acc2c215032a17482273a2ca00d90e5`

The fork history contains upstream-authored commits inherited from the BLAKE3 repository. This does not make those historical upstream changes RMR-authored.

Selected upstream CI changes observed in path history:

| Date UTC | Commit | GitHub author | GitHub committer | Observed commit subject |
|---|---|---|---|---|
| 2026-08-03 | `9eac279f...` | `oconnor663` | `oconnor663` | use vswhere to find Visual Studio in CI |
| 2026-07-27 | `fc3d0e98...` | `nazar-pc` | `nazar-pc` | Fix path to Visual Studio toolchain in CI |
| 2026-04-24 | `6a45feed...` | `oconnor663` | `oconnor663` | add LTO builds to CI |
| 2025-04-17 | `70c8fe96...` | `silvanshade` | `BurningEnlightenment` | Add C++ std lib to pkg-config |
| 2025-04-03 | `bafe693a...` | `silvanshade` | `BurningEnlightenment` | Add CI tests for pkg-config |
| 2025-03-18 | `4011d240...` | `oconnor663` | `oconnor663` | add wasm tests to CI |
| 2023-12-30 | `4d32708f...` | `striezel` | `oconnor663` | replace unmaintained actions-rs/toolchain action in CI |
| 2023-12-28 | `5306464d...` | `striezel` | `BurningEnlightenment` | update actions/checkout in GitHub Actions to v4 |

## Fork synchronization event

Commit `774ba7ec9639a79132a6992e35f16035707f58d9`:

- GitHub author: `rafaelmeloreisnovo`
- GitHub committer: `rafaelmeloreisnovo`
- subject: `sync(rmr): align fork baseline with upstream BLAKE3 1.8.7`

Its diff synchronized, among other items:

- native Linux arm64 matrix entries;
- LTO build checks;
- MSRV toolchain update;
- current Visual Studio discovery using `vswhere`.

## Responsibility boundary

- Upstream contributors above: factual authors/committers of the listed upstream CI commits.
- `rafaelmeloreisnovo`: factual author/committer of the fork synchronization commit `774ba7e...`.
- No statement here assigns fault, intent, maintenance duty, or legal responsibility beyond the Git records.

## R3

- F_ok: upstream-vs-fork CI provenance separated.
- F_gap: RMR-specific concurrency history still unresolved.
- F_next: reconstruct the concurrency policy sequence commit by commit.
