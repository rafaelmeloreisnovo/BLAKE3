<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.

Forensic evidence ledger. Facts only; hypotheses remain explicitly separated.
-->

# BLAKE3 — Closed PR / Input Provenance Evidence Ledger V1

**Repository:** rafaelmeloreisnovo/BLAKE3  
**Upstream audited:** BLAKE3-team/BLAKE3  
**Date:** 2026-09-24  
**Base fork commit:** 3602c95ca22f26440cfca1a8e2b7396847122927  
**Class:** FORENSIC_EVIDENCE_LEDGER / APPEND_ONLY / FAIL_CLOSED  
**Rule:** SOURCE != ARTEFACT != EXECUTION != EVIDENCE != CLAIM  
**Claim gate:** no allegation of plagiarism, fraud, collusion, suppression, bot network, or intent is promoted without direct evidence.

## 1. Scope and method

This ledger records verified public GitHub facts concerning:

1. pull requests that are closed and reported by GitHub as `merged=false`;
2. issue/problem reports that act as technical inputs before later implementation;
3. who opened, closed, reopened, referenced, implemented, authored, or committed changes;
4. cases where code/functionality reached upstream through a route other than a conventional PR merge;
5. explicit use of automation/apps where GitHub records it.

The forensic unit is not only a final commit. Where evidence exists, the chain is preserved as:

```text
PROBLEM/INPUT
  -> DIAGNOSIS
  -> PROPOSED SOLUTION
  -> PR/PATCH
  -> REVIEW/DISCUSSION
  -> IMPLEMENTATION
  -> CLOSE/MERGE/REPLACEMENT
  -> RELEASE
```

A current PR body is not assumed to be its original historical text. Stronger anchors are Git object SHAs, event timestamps, commit authorship/committer metadata, comments, cross-references, and release records.

## 2. Enumerated closed / non-merged PR corpus

The audit enumerated **63 unique pull requests** in BLAKE3-team/BLAKE3 that were observed as `closed` and `merged=false` in the queried historical set.

For **62/63**, the first-page issue-event history exposed an explicit close actor. One item (#185) had a closed PR state but the close event was not present in the retrieved first-page event surface; that actor remains TOKEN_VAZIO in this ledger.

Observed close actors are not interpreted as intent. A close event proves only that the named GitHub actor performed the close operation recorded by GitHub.

## 3. Rafael Melo Reis — PR #533

Primary URL: https://github.com/BLAKE3-team/BLAKE3/pull/533

Verified metadata:

```text
number      = 533
title       = "A little bit of helping"
author      = rafaelmeloreisnovo
created_at  = 2025-11-25T00:42:22Z
closed_at   = 2025-11-25T03:29:05Z / event at 03:29:06Z
merged      = false
head_sha    = 15829f851e45d1327b017be67a7b88d7725bc653
head_repo   = rafaelmeloreisnovo/BLAKE3
close_actor = oconnor663
```

Event history additionally records:

```text
2025-11-25T00:42:23Z  rafaelmeloreisnovo -> review_requested(Copilot)
2025-11-25T00:43:02Z  copilot_work_started via copilot-pull-request-reviewer
2025-11-25T00:45:22Z  copilot_work_finished
2025-11-25T01:08:04Z  rafaelmeloreisnovo -> review_requested(Copilot)
2025-11-25T01:08:31Z  copilot_work_started
2025-11-25T01:10:10Z  copilot_work_finished
2025-11-25T03:29:06Z  oconnor663 -> closed
```

The Copilot events are classified by GitHub as bot/app activity, but the review was requested by `rafaelmeloreisnovo`. The close event itself was performed by `oconnor663`, classified by GitHub as a User.

Allowed conclusion:

```text
PR533_PUBLIC_PRIOR_ART = VERIFIED_PRIMARY
PR533_CLOSED_BY_OCONNOR663 = VERIFIED_PRIMARY
PR533_COPILOT_REVIEW_ACTIVITY = VERIFIED_PRIMARY
COORDINATED_BOT_ACTION_AGAINST_PR533 = TOKEN_VAZIO
```

## 4. Rafael Melo Reis — PR #589 control

Primary URL: https://github.com/BLAKE3-team/BLAKE3/pull/589

Verified metadata:

```text
number      = 589
author      = rafaelmeloreisnovo
created_at  = 2026-09-23T07:51:50Z
closed_at   = 2026-09-23T07:59:18Z
merged      = false
head_sha    = 809f6213674798cbcd0e76f3fbf060ebe1d68a1d
close_actor = rafaelmeloreisnovo
```

This is a control demonstrating why `closed && merged=false` cannot by itself be treated as maintainer rejection.

## 5. Verified pattern: closed/non-merged PR != unused contribution

### 5.1 PR #44 -> different maintainer implementation of b3sum --check

Primary PR: https://github.com/BLAKE3-team/BLAKE3/pull/44  
Related issue: https://github.com/BLAKE3-team/BLAKE3/issues/33

PR #44 was opened by `phayes` on 2020-01-26 for `b3sum --check`.

The review discussion contains technical requirements concerning:

- unusual filenames;
- newline/backslash escaping;
- Unicode;
- Windows path behavior;
- hash length;
- keyed/derive-key incompatibilities;
- security consequences of accepting short hashes.

Later upstream commits include:

```text
dc2a79d2669c92795c7214761a0568d629a100e4
2020-05-05
"add newline and backslash escaping to b3sum output"

c6a99dbb239522b1f99950faefc2b072ee3b1a53
2020-05-12
"add parse_check_line"

c5c07bb337d0af7522666d05308aaf24eef3709c
2020-05-13
"refactor b3sum to support --check"
```

On 2020-05-15, `oconnor663` closed #44 and stated in the PR discussion that a **different `--check` implementation** had been pushed to master.

Classification:

```text
CLOSED_PR_FOLLOWED_BY_DIFFERENT_IMPLEMENTATION = VERIFIED_PRIMARY
PUBLIC_DISCLOSURE_OF_DIFFERENT_IMPLEMENTATION = VERIFIED_PRIMARY
HIDDEN_APPROPRIATION = NOT_ESTABLISHED
```

### 5.2 PR #118 -> direct maintainer fix with explicit credit

Primary PR: https://github.com/BLAKE3-team/BLAKE3/pull/118

Event chain:

```text
pascal-cuoq -> closed
oconnor663   -> reopened
oconnor663   -> referenced commit 0b13637ae31c2e7e1a471e39258606fabb01685e
pascal-cuoq -> closed
```

Commit:

```text
0b13637ae31c2e7e1a471e39258606fabb01685e
"fix a couple of big-endianness mistakes in blake3.c"
```

The commit explicitly gives kudos to `@pascal-cuoq` and `@jakub-zwolakowski` and links the original report.

Classification: `MAINTAINER_IMPLEMENTATION_WITH_EXPLICIT_CREDIT`.

### 5.3 PR #247 -> CMake functionality reaches upstream despite merged=false

Primary PR: https://github.com/BLAKE3-team/BLAKE3/pull/247  
Author: `SteveGremory`  
Close actor: `oconnor663`  
State: `merged=false`

Relevant upstream commits:

```text
1569e345552187a6640b2eaff540a84b525854e2
"Added CMake support, CMakeLists.txt taken from issue 102"

b0a3863c06584d9a1a50d5a8c51ae88d7f8ecb96
"Minor changes to CMake, added SSE support..."

d7f43a339a58a5a47db181bf150db1eeb80f9a9c
"Added CI Support for the CMake build..."
```

The first commit preserves `SteveGremory` as Git author while GitHub/O'Connor performs integration as committer.

Classification: `CONTENT_INTEGRATED_OUTSIDE_CONVENTIONAL_PR_MERGE_WITH_AUTHORSHIP_PRESERVED`.

### 5.4 PR #341 -> WASM SIMD incorporated and release credit preserved

Primary PR: https://github.com/BLAKE3-team/BLAKE3/pull/341  
Author: `monoid`  
State: `merged=false`  
Close actor: `oconnor663`

Upstream commit:

```text
d4aed8145b5478d62308319d2ad849e2a1e371db
"Wasm32 SIMD implementation"
```

Release commit:

```text
a9e92981a5c15db7f6c3a040ffc764b7a49eb4d4
version 1.7.0
```

The release text explicitly credits `@monoid (#341)`.

Classification: `NON_MERGED_PR_WITH_EXPLICIT_RELEASE_CREDIT`.

### 5.5 PR #358 -> successor PR #359

PR #358 by `rui314` proposed an ARM32 NEON flag fix. A successor PR #359, by another author, states that it uses a **different approach to fix the issue of #358** and was merged shortly after.

Classification: `SHORT_WINDOW_SUCCESSOR_WITH_EXPLICIT_ATTRIBUTION`.

### 5.6 PR #430 -> successor #453 / release credits both

PR #430 by `dbohdan` proposed `b3sum --tag`. A later implementation path through #453 was incorporated. BLAKE3 1.7.0 credits both `@leahneukirchen (#453)` and `@dbohdan (#430)`.

Classification: `SUCCESSOR_IMPLEMENTATION_WITH_DUAL_CREDIT`.

### 5.7 PR #477 -> same-author rework #483

PR #477 by `silvanshade` was closed. PR #483 was a refactor/rework of #477 by the same contributor and became the successor path.

Classification: `SAME_AUTHOR_REWORK`.

### 5.8 PR #487 -> maintainer successor #570 with explicit attribution

PR #487 by `nabijaczleweli` concerned mmap conditions. A later maintainer implementation (#570) explicitly states that it was **originally suggested by @nabijaczleweli in #487**.

Classification: `MAINTAINER_SUCCESSOR_WITH_EXPLICIT_ATTRIBUTION`.

### 5.9 PR #296 / issue #290 -> serde support

Issue #290 requested optional serde support before PR #296. PR #296 proposed a serde feature and was closed as non-merged.

Later upstream commit:

```text
5e3eb949a78f767a5a458022afedeb3e66398659
2023-09-19
"Add serde support for Hash behind optional feature"
```

Because issue #290 predates #296, the public input lineage has multiple antecedents.

Classification:

```text
LATER_FUNCTIONAL_EQUIVALENT = VERIFIED_PRIMARY
DERIVATION_FROM_PR296_SPECIFICALLY = TOKEN_VAZIO
```

### 5.10 PR #380 / issue #379 -> commit-linked closure

Issue #379 reported a GCC `-Werror=logical-op` failure. PR #380 proposed the correction.

Upstream commit:

```text
2918c51bc693bd8608e04c790e1fccfabeb40514
"silenc gcc Werror=logical-op"
```

The commit message links issue #379 and closes PR #380; the issue-event history associates the close with the same commit.

Classification: `DIRECT_EXPLICIT_PROVENANCE`.

## 6. Inputs/issues as pre-implementation technical artifacts

The audit verified that BLAKE3 issues frequently contain enough detail to act as technical inputs before a final patch exists.

Examples:

| Input | Date | Technical content | Later path |
|---|---|---|---|
| #23 | 2020-01-13 | mmap performance problem on small files | #26: do not mmap below 16 KiB |
| #32 | 2020-01-18 | b3sum performance problem | feeds mmap/performance investigation |
| #35 | 2020-01-19 | concrete prefetch optimization proposal/diff | prefetch investigation |
| #39 | 2020-01-23 | NEON prefetch optimization question | NEON performance line |
| #69 | 2020-02-25 | reproducible update performance cliff | later update-path work |
| #310 | 2023-06-08 | NEON benchmark on Raspberry Pi 4 | ARM/NEON optimization context |
| #350 | 2023-09-24 | ARMv7+NEON build/assertion diagnosis | #353 explicitly fixes #350 |
| #379 | 2024-01-29 | GCC logical-op failure with compiler log | #380 / commit 2918c51... |
| #478 | 2025-04-24 | SIMD for multiple independent values/Merkle workload | parallel/SIMD investigation |
| #484 | 2025-05-03 | RISC-V SIMD with benchmarks | RISC-V vector work |
| #494 | 2025-07-12 | Cygwin missing blake3_xof_many_avx512 | #496 workaround |
| #550 | 2026-04-24 | AArch64 LTO linker failure | #552 explicitly fixes #550 |
| #576 | 2026-08-13 | NEON xof_many design, implementation description, ~2x benchmark | #577 by same author |
| #583 | 2026-08-28 | C prefetch pointer arithmetic UB, reproducer and addresses | open at audit time |

Therefore, for authorship/provenance analysis:

```text
FINAL_COMMIT_ONLY
```

is an incomplete model. The evidence graph must preserve problem reporters, diagnostic contributors, proposed solutions, patch authors, reviewers, integrators, and release attribution independently.

## 7. Short-window temporal candidate near PR #533: cryptopp-modern

BLAKE3 PR #533 was published 2025-11-25.

Upstream issue #534 was opened 2025-12-01 by `Coralesoft` concerning BLAKE3 SIMD performance on MSVC versus GCC.

The referenced external repository `cryptopp-modern/cryptopp-modern` has a BLAKE3 implementation commit:

```text
1fb17a2cdf3d4203cb92d6321b365b06cc7d549d
2025-10-30
"Add BLAKE3 cryptographic hash function"
```

This predates PR #533.

A separate SIMD acceleration commit exists:

```text
73a813416e648662f1659a57284aeefc5a924215
2025-11-29
"Add BLAKE3 SSE4.1/AVX2 parallel chunk processing"
```

This postdates PR #533 by approximately four days. It adds SSE4.1/AVX2 parallel chunk hashing, runtime dispatch, build-system changes, and benchmarking. The new `blake3_simd.cpp` states that its SSE4.1 implementation is based on the BLAKE3 team's reference implementation.

Allowed classification:

```text
CRYPTOPP_BLAKE3_BASE_PREDATES_PR533 = VERIFIED_PRIMARY
CRYPTOPP_SIMD_COMMIT_POSTDATES_PR533 = VERIFIED_PRIMARY
TEMPORAL_AND_TOPIC_OVERLAP = VERIFIED_PRIMARY
DERIVATION_FROM_PR533 = TOKEN_VAZIO
```

## 8. Explicit automation evidence

Within the audited closed/non-merged PR set, two material automation/app observations were located:

### #533

GitHub records Copilot review events. They follow explicit review requests by `rafaelmeloreisnovo`.

### #495

PR #495 was opened by `oconnor663` and GitHub records an event performed via the `chatgpt-codex-connector` app. The PR body also includes a ChatGPT/Codex task URL.

This proves that AI-assisted tooling participated in at least some public development workflow events.

It does **not** prove:

- a coordinated bot network;
- automatic surveillance of contributor forks;
- copying;
- authorship laundering;
- intentional suppression.

Those remain `TOKEN_VAZIO` unless direct evidence is located.

## 9. Forensic invariants established by this pass

```text
CLOSED != REJECTED_IDEA
MERGED_FALSE != NOT_INCORPORATED
PR_AUTHOR != COMMIT_AUTHOR != COMMITTER != CLOSE_ACTOR
ISSUE_REPORTER != DIAGNOSTIC_AUTHOR != PATCH_AUTHOR != INTEGRATOR
TEMPORAL_PROXIMITY != CAUSAL_DERIVATION
BOT_EVENT != BOT_NETWORK
MAINTAINER_REIMPLEMENTATION != HIDDEN_APPROPRIATION
EXPLICIT_CREDIT != ABSENCE_OF_PROVENANCE
```

## 10. Claim ledger

| Claim | State |
|---|---|
| BLAKE3 has closed/non-merged PRs whose functionality later reaches upstream by other routes | PROVADO |
| Some such routes preserve original Git authorship | PROVADO |
| Some successor implementations explicitly cite earlier PRs/authors | PROVADO |
| A maintainer has publicly closed a PR after pushing a different implementation of the same feature (#44) | PROVADO |
| Issues can contain detailed technical inputs before implementation | PROVADO |
| PR #533 was closed by oconnor663 | PROVADO |
| PR #533 had Copilot review activity explicitly requested by Rafael | PROVADO |
| PR #495 contains a GitHub event via chatgpt-codex-connector | PROVADO |
| Closed/non-merged status alone reconstructs contribution history | REFUTADO |
| All later equivalent changes derive from the earlier closed PR | TOKEN_VAZIO |
| A coordinated bot network controls BLAKE3 contribution handling | TOKEN_VAZIO |
| Maintainers coordinate to suppress collision/security criticism | TOKEN_VAZIO pending separate evidence audit |

## 11. Next forensic gate

A separate evidence pass is required before any coordination/suppression claim can move from TOKEN_VAZIO. That pass must examine:

```text
collision / collision resistance
tree hashing / Merkle structure
domain separation / flags
XOF / counter domain
incremental boundaries
length / encoding ambiguity
proofs / Bao lineage
security reports
issue close/reopen/delete/reference events
review actors
maintainer comments
commits and release notes
cross-repository prior art
```

The result must distinguish:

```text
ordinary maintainer concentration
technical disagreement
duplicate/known issue
security triage
reimplementation
moderation
attribution omission
actual coordinated suppression
```

Only the last classification requires direct, independent evidence of coordination and suppression.

---

**R3:** F_ok = closed/non-merged corpus enumerated; event actors and multiple alternative-integration chains verified; issue-as-input model established; automation facts bounded. F_gap = exhaustive semantic successor scoring and coordination/suppression evidence not yet established. F_next = collision/tree/domain/security coordination audit using immutable Git/event evidence.
