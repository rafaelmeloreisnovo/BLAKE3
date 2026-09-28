# MANIFESTO CANÔNICO — BLAKE3 CHECK-CHAIN CUSTODY V1

- schema: RMR_CHECK_CHAIN_CUSTODY_V1
- date: 2026-09-27
- clock: TOKEN_VAZIO
- repository: `rafaelmeloreisnovo/BLAKE3`
- audited_master: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- parent_ledger_commit: `cb81b7b2d796d193d5f4ab2c67a839097b31af14`
- canonical_status: PROPOSED_ON_AUDIT_BRANCH
- claim_allowed: false

## 1. Invariants

`SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM`

`TOKEN_VAZIO != 0`

`CANCELLED != FAIL_TEST`

`FIRST_PAGE_CHECKS != COMPLETE_CHECK_SET`

`PR_HEAD != MERGE_COMMIT`

No fact in this manifesto assigns motive, fault, negligence, legal liability, or security compromise. Identity is attributed only where GitHub records authorship, commit, merge, or execution.

---

## 2. Scope

This manifesto covers the check-chain involved in the observed sequence “push accepted, visible checks pass, later commit/check state becomes red”, including:

- upstream BLAKE3 `.github/workflows/ci.yml`;
- fork synchronization of that workflow;
- RMR full-validation aggregation;
- RMR comprehensive aggregation;
- RMR concurrency/supersession policy;
- CF140 freestanding workflow and ELF auditor;
- ZIP custody gate;
- GitHub Checks enumeration behavior.

---

## 3. Current master acceptance boundary

At audited master `b1c8cab1...`:

- branch protection on `master`: disabled;
- required status checks: none;
- repository rulesets returned: none.

Therefore the repository presently permits a push/merge commit to exist on `master` before GitHub Actions has finished. A later red check is a post-acceptance CI state, not evidence that the original push was rejected.

---

## 4. Upstream CI provenance

The current fork `.github/workflows/ci.yml` and current `BLAKE3-team/BLAKE3` file were observed with identical blob SHA:

`f7dd8c053acc2c215032a17482273a2ca00d90e5`

Observed upstream contributors in the CI path history include, among others:

- Jack O'Connor / `oconnor663`;
- Nazar Mokrynskyi / `nazar-pc`;
- Dirk Stolle / `striezel`;
- `silvanshade`;
- Henrik Gaßmann / `BurningEnlightenment`.

These identities are factual authors/committers of specific upstream commits. They are not attributed responsibility for RMR-specific workflows.

Fork synchronization event:

`774ba7ec9639a79132a6992e35f16035707f58d9`
- author: `rafaelmeloreisnovo`
- committer: `rafaelmeloreisnovo`
- synchronized the fork CI baseline with upstream BLAKE3 1.8.7 changes.

---

## 5. RMR workflow provenance and responsibility boundary

The relevant RMR-specific workflow/configuration commits inspected are attributed by GitHub to `rafaelmeloreisnovo`, except where GitHub `web-flow` appears as merge committer.

### Full validation

Created:
`978e1362ac61c95ad8623555f724da577afa3d19`

### Comprehensive V3

Created:
`dd8fe7174c76ab95b47b3a12b7bd179dd9271d15`

### Global supersession

`141f8065e2c3845978cff7aabb6eeadebc155d30`
- author: `rafaelmeloreisnovo`
- committer: `web-flow`
- changed both large workflows from PR/ref grouping to global grouping.

### Exact-head isolation

`603fdda064d919521cdebeea8ce1c8a760795cf0`
- author/committer: `rafaelmeloreisnovo`
- appended exact head SHA to RMR concurrency groups.

### PR/ref supersession restored

`1f44afd6c7ba043ace007905f0904b13b68a8d5e`
- full validation.

`12c48f0a405046abe0079b472785869b3fb5eb8a`
- comprehensive V3.

Both are authored/committed by `rafaelmeloreisnovo`.

Current result: consecutive pushes to the same branch/ref can supersede earlier runs because `cancel-in-progress: true` remains enabled.

---

## 6. Late FAIL mechanism from cancelled producers

Current large RMR workflows combine:

- PR/ref concurrency grouping;
- `cancel-in-progress: true`;
- terminal aggregator with `if: always()`;
- artifact download;
- fail-closed completeness assertions.

Observed on commit `29b1a651...`:

### Full validation run `36286488994`

- producer jobs became cancelled after a newer master push;
- terminal `aggregate` still executed;
- artifact download found 0 artifacts;
- report became `execution_complete=false`;
- report became `PARTIAL`;
- required axes became `TOKEN_VAZIO_NOT_FOUND`;
- completeness assertion failed.

### Comprehensive run `36286489003`

Same structural sequence:
- cancelled producers;
- `summary` ran because of `always()`;
- 0 artifacts;
- `PARTIAL`;
- required axes missing;
- assertion failed.

Canonical interpretation:

`CANCELLED_SUPERSEDED -> EVIDENCE_INCOMPLETE -> TERMINAL_GATE_FAIL`

This is not semantically identical to:

`TEST_EXECUTED -> DEFECT_OBSERVED -> FAIL_TEST`

The UI and future receipts should preserve that distinction.

---

## 7. Check enumeration / apparent hidden state

For audited commit `b1c8cab1...`:

- authoritative response reported `total_count=92`;
- an incomplete page exposed only 30 returned check objects;
- those 30 were all successful;
- complete enumeration with `per_page=100` exposed all 92;
- complete state was 91 success + 1 failure.

Sole observed failure:

- workflow: `rmr-crypto-freestanding140-v1`
- check: `freestanding140`
- run: `36287272138`
- job: `108530284178`

The legacy combined-status endpoint did not expose these GitHub Actions checks as legacy status contexts.

Canonical tooling rule:

A commit cannot be promoted to complete PASS from the first check-runs page or from legacy combined-status alone.

---

## 8. CF140 terminal FAIL classification

Workflow introduced in:
`1bcc39e1f7ecaa04f9615516e4300cc3d60bf18f`

ELF audit tightened in:
`b663e43bfc7070c749d076810b3c0147f4e8b278`

Both are authored/committed by `rafaelmeloreisnovo`.

Observed on current master:

- host selftest: PASS;
- SHA-256 `abc` compression KAT: PASS;
- source-shape gate: PASS;
- linker invocation included `--no-undefined`;
- artifact audit then saw symbol-table row 0:
  `0: 0000000000000000 0 NOTYPE LOCAL DEFAULT UND`;
- predicate treated every field-7 `UND` row as bad;
- workflow emitted `FAIL unexpected_UND`.

ELF specification cross-check establishes symbol-table index 0 as reserved `STN_UNDEF`, with no name, local binding, no type, and `SHN_UNDEF`.

Canonical classification:

- external unresolved named symbol: NOT_PROVEN by the observed row;
- KAT defect: NOT_OBSERVED;
- source-shape defect: NOT_OBSERVED;
- auditor false positive on reserved symbol 0: SUPPORTED_BY_SOURCE + RUNTIME + ELF_SPEC.

No source fix is part of this manifesto commit.

---

## 9. ZIP custody late-gate history

Workflow created:
`430ad910d13ff2f46740a0e30fecabe7b42c112b`

Dependency/cache reduction:
`2c1caf90b940f1b408851e0bc845cf54a70e8072`

Forensic-time tests added:
`580132ce38edecf3b9138e86457780c6d9980f0a`

Authorship gate scoped:
`3885e67431fff77ce21ce15be746e91016dd2f8f`

Exact-head checkout:
`470b6d4037af046b65c7a7bdde7230f6769de29e`

These commits are attributed by GitHub to `rafaelmeloreisnovo`.

Observed run `36286488972` on `29b1a651...`:

- ZIP custody profile: PASS;
- seven ZIP custody tests: PASS;
- ten forensic-time tests: PASS;
- later authorship-boundary step failed on:
  `MISSING_HEADER: rmr/crypto/runtime/receipts/RMR_CRYPTO_V3_PROVIDER_CROSSCHECK_20260924.json`.

Canonical interpretation:
the terminal workflow failure was a governance/header failure after functional/contract tests had passed.

---

## 10. Responsibility map

| Layer | Factual responsibility/provenance observed |
|---|---|
| Upstream BLAKE3 CI history | individual upstream Git authors/committers recorded in BLAKE3-team history |
| Fork CI synchronization | `rafaelmeloreisnovo`, commit `774ba7e...` |
| RMR full-validation workflow | `rafaelmeloreisnovo` commit history |
| RMR comprehensive workflow | `rafaelmeloreisnovo` commit history |
| RMR concurrency policy changes | `rafaelmeloreisnovo`; `web-flow` only where recorded as merge committer |
| CF140 workflow/auditor | `rafaelmeloreisnovo` commit history |
| ZIP custody workflow | `rafaelmeloreisnovo` commit history |
| Workflow execution service | GitHub Actions application/runner |
| Merge technical committer where applicable | GitHub `web-flow` |
| Motive / intent / legal fault | TOKEN_VAZIO — not inferred |

---

## 11. Corrective order derived from evidence

These are recommendations, not executed changes in this manifesto:

1. CF140 auditor: exclude the reserved `STN_UNDEF` entry 0 while continuing to reject named unresolved symbols.
2. Aggregators: preserve `CANCELLED_SUPERSEDED` separately from `FAIL_TEST`; do not let `always()` erase cause.
3. Evidence report: include producer `needs.*.result` before artifact completeness assertions.
4. RafGitTools: exhaust Checks API pagination and reconcile `returned_count` with `total_count`.
5. RafGitTools: distinguish legacy Status API from Checks API.
6. Add one explicit terminal canonical check context that summarizes all required gates with typed causes.
7. Only after terminal semantics are stable, consider branch protection/rulesets requiring that canonical terminal check.

---

## 12. Ledger chain

- μ0001 source snapshot — commit `7c3c9c87f4b1c0d360a3c4bd97636d7926f48b25`
- μ0002 upstream CI provenance — commit `9615d76b24681a99a213c311002f9bbf805eb010`
- μ0003 concurrency history — commit `4e08283e8b264cecf7d9929a841474a734f8309a`
- μ0004 cancelled -> late FAIL — commit `b25f2eec444799bb3cefffe4f12acc8c05b71f13`
- μ0005 check enumeration/pagination — commit `0ea4b41801e8a39fa4c5a0689f5bd818ead23102`
- μ0006 CF140 ELF gate — commit `4de3fb09d8e2a4c8aed42a55e969d7e1c2842746`
- μ0007 ZIP custody history — commit `cb81b7b2d796d193d5f4ab2c67a839097b31af14`

Each entry is append-only in the audit branch and is linked by its recorded parent commit.

---

## 13. R3

`F_ok`
- history reconstructed;
- author/committer boundaries recorded;
- upstream and RMR provenance separated;
- post-push late-failure mechanisms reproduced from logs;
- current sole failing check identified and classified.

`F_gap`
- source fixes are not yet applied;
- independent reproduction of proposed fixes is NOT_RUN;
- master has no canonical required terminal check;
- this manifesto is not yet merged into master.

`F_next`
- create a final chain receipt for this audit branch;
- open a reviewable PR to master;
- only then implement corrective code as a separate successor delta.

`claim_allowed=false`
