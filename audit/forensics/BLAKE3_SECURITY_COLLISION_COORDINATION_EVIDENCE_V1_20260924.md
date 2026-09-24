<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.

Forensic security-governance evidence ledger.
No intent or misconduct claim is promoted beyond the public evidence.
-->

# BLAKE3 — Security / Collision / Tree-Hash Coordination Evidence V1

**Repository audited:** BLAKE3-team/BLAKE3  
**Destination:** rafaelmeloreisnovo/BLAKE3  
**Date:** 2026-09-24  
**Fork base for this audit:** 3602c95ca22f26440cfca1a8e2b7396847122927  
**Class:** FORENSIC_COORDINATION_EVIDENCE / FACTS_ONLY / FAIL_CLOSED  
**Claim rule:** observed coordination is separated from motive; maintainer concentration is not by itself evidence of suppression or collusion.

## 0. Question tested

The audit tests whether the public GitHub record supports a claim that BLAKE3 maintainers/designers coordinate so that outside contributors cannot publicly discuss collision-resistance, tree-hashing, or related mathematical/security limitations.

The test is deliberately falsifiable. Evidence that maintainers publish, preserve, or expand such criticism counts against a suppression hypothesis.

## 1. Official designer set

The upstream README states:

```text
BLAKE3 was designed by:
- @oconnor663 (Jack O'Connor)
- @sneves (Samuel Neves)
- @veorq (Jean-Philippe Aumasson)
- @zookozcash (Zooko)
```

The README also states that BLAKE3 development was sponsored by Electric Coin Company.

This establishes a documented designer network. It does not establish a hierarchy, "chief", common intent, or coordinated misconduct.

## 2. Direct collision/security issue population sampled

The audit inspected the public state/event surface of the following collision/security/tree-related issues:

```text
#13, #70, #98, #107, #123, #138, #163, #168, #194,
#208, #246, #274, #278, #396, #441, #504, #82, #436
```

Observed state:

```text
18 issues examined
8 open
10 closed
0 locked
0 active_lock_reason
```

For the 10 closed issues:

```text
closed by oconnor663: 6
closed by original reporter: 3
closed actor unavailable on current public event surface: 1 (#504)
```

No issue in this set was observed as locked.

This does not prove that no historical content was ever deleted or edited. Current GitHub issue/event surfaces do not provide a complete immutable history of every body edit or deleted object; those remain TOKEN_VAZIO unless recovered from independent archives.

## 3. Collision resistance discussion #168 -> documentation PR #169

Issue:
https://github.com/BLAKE3-team/BLAKE3/issues/168

Title:
`Please document collision-resistance properties of BLAKE3 with less than 256 bits`

Created by `joshtriplett` on 2021-04-28.

Observed coordination:

1. `oconnor663` publicly answered the collision-resistance questions.
2. He explicitly asked `@sneves` and `@veorq` to weigh in.
3. `sneves` publicly responded on the security claim.
4. `joshtriplett` requested documentation.
5. `oconnor663` opened PR #169 and asked `@sneves` to review the wording.
6. PR #169 was merged.
7. Issue #168 was then closed by `oconnor663`.

PR:
https://github.com/BLAKE3-team/BLAKE3/pull/169

Verified merge commit:

```text
7cd208afcf91f69b786549a4bed77371a7b9cc2d
"explicitly document the properties of short outputs"
```

The commit message explicitly states that the change was suggested by `@joshtriplett` in issue #168.

The merged documentation states, in substance, that for output sizes up to 256 bits, an N-bit BLAKE3 output is intended to provide N/2 bits of collision resistance and that shorter outputs provide less security.

Classification:

```text
COAUTHOR_SECURITY_COORDINATION = PROVADO
OUTSIDER_REQUEST -> PUBLIC_DOC_CHANGE = PROVADO
ORIGINAL_REPORTER_CREDIT = PROVADO
SUPPRESSION_OF_COLLISION_DISCUSSION_IN_THIS_CHAIN = CONTRADICTED_BY_RECORD
```

## 4. Collision discussion #194

Issue:
https://github.com/BLAKE3-team/BLAKE3/issues/194

Title:
`BLAKE3 collision resistance`

Created by `abitrolly` on 2021-09-01.

The thread contains direct discussion of:

- the `2^128` full-output collision-resistance target;
- truncated 128-bit outputs and approximately `2^64` generic collision work;
- birthday-bound reasoning;
- use for content addressing;
- practical brute-force cost and the fact that 64 bits of collision security is not large by state/company-scale standards.

Participants include `sneves`, `oconnor663`, `elichai`, and the reporter.

The issue had 14 comments and was closed by **the reporter `abitrolly`**, not by a maintainer.

It is not locked.

Classification:

```text
PUBLIC_CRITICAL_COLLISION_DISCUSSION = PROVADO
SELF_CLOSE_BY_REPORTER = PROVADO
MAINTAINER_SUPPRESSION_IN_THIS_CHAIN = NOT_OBSERVED
```

## 5. Issue #441 explicitly discusses collisions inside the BLAKE3 tree

Issue:
https://github.com/BLAKE3-team/BLAKE3/issues/441

Title:
`Why is the security limited to 256-bit?`

Created 2025-01-07 and still open at the audit date.

`oconnor663` publicly explains that BLAKE3's state is a tree and states that if an attacker can find a collision anywhere in that tree, it can lead to output collisions; the 256-bit chaining values therefore bound the security.

The issue is open and not locked.

Classification:

```text
TREE_COLLISION_LIMITATION_PUBLICLY_DISCLOSED = PROVADO
THREAD_OPEN = PROVADO
THREAD_LOCKED = false
```

## 6. External security finding -> merged security documentation: PR #227

PR:
https://github.com/BLAKE3-team/BLAKE3/pull/227

Title:
`document the extended output security issue found by Aldo Gunsing`

Opened by `oconnor663` on 2022-03-02 and merged on 2022-03-03.

The PR directly links Aldo Gunsing's paper:
https://eprint.iacr.org/2022/283

A constituent upstream commit is:

```text
ea3bc782d8128d7f52008d459ecd4df8b51979cf
"document the extended output security issue found by Aldo Gunsing"
```

The merged security notes explicitly state that callers should not rely on secrecy of the XOF output offset and cite the external work.

PR #227 merge/head lineage observed:

```text
head_sha / merge_commit_sha = d295410aad19d70e4b7ab9b93b216a77fb8d40e4
merged = true
merge_actor = oconnor663
```

Classification:

```text
EXTERNAL_SECURITY_FINDING_PUBLICLY_NAMED = PROVADO
SECURITY_LIMITATION_ADDED_TO_OFFICIAL_DOCS = PROVADO
SUPPRESSION_OF_THIS_SECURITY_FINDING = CONTRADICTED_BY_RECORD
```

## 7. Tree hashing discussion remains public and evolves into official API

### Issue #82

https://github.com/BLAKE3-team/BLAKE3/issues/82

Opened 2020-04-27; still open at audit time; 14 comments; not locked.

The thread discusses:

- intermediate tree nodes;
- incremental verification;
- subtree chaining values;
- Bao;
- custom tree structures;
- chunk-size tradeoffs;
- misuse risks around non-root finalization;
- external fork code from Fleek Network.

`oconnor663` repeatedly directs users to `oconnor663/bao`, a maintainer-owned project. This is a factual reference relationship.

A Fleek Network contributor later posted an external BLAKE3 fork and implementation in the thread. The thread remained public.

### Issue #436

https://github.com/BLAKE3-team/BLAKE3/issues/436

Opened 2024-12-03; still open; not locked.

The issue asks for a public tree-hashing API. `oconnor663` acknowledges the undocumented `guts` API and links an external fork, `n0-computer/iroh-blake3`.

Later he reports that BLAKE3 v1.8.0 added the documented `blake3::hazmat` module.

## 8. External tree/fork inputs -> official hazmat API

Relevant earlier external PR:

https://github.com/BLAKE3-team/BLAKE3/pull/329

Author: `rklaehn`  
Created: 2023-07-29  
State at audit time: open / not merged.

The PR proposes efficient non-root subtree hashing and explicitly references Bao, abao, bao-tree, and a Fleek Network BLAKE3 fork.

In its discussion, a maintainer account describes a separate `guts_api` branch already in progress and names maintainer-owned Bao, Bessie, and BLAKE3-AEAD as intended consumers.

Later official PR:

https://github.com/BLAKE3-team/BLAKE3/pull/458

Title:
`add the hazmat module`

Author: `oconnor663`  
Opened: 2025-03-27  
Merged: 2025-03-31  
Head SHA: `9004cbcbde679ac5f8e24d698d2aa7e17d848d17`

The PR body explicitly:

- CCs `@rklaehn` and `@qti3e`;
- states that the goal is to prevent projects like Bao from depending on undocumented APIs;
- states that projects like Iroh should no longer need their own BLAKE3 forks;
- links `n0-computer/iroh-blake3`.

The external contributors publicly test and discuss the API.

Merged implementation commit:

```text
e1c2ea27fdd717fd924d7b286a125408d7e817f7
"add the hazmat module and deprecate the undocumented guts module"
```

Release commit:

```text
00c2ea974d33d19d91d8de3c12ff8c8eb1fc8dbd
"version 1.8.0"
```

The release notes state that `hazmat` is intended for advanced uses such as Bao and Iroh.

Classification:

```text
EXTERNAL_TREE_API_PRESSURE/INPUT = PROVADO
MAINTAINER_OWN_API_LINEAGE = PROVADO
EXTERNAL_CONTRIBUTORS_EXPLICITLY_CC'D = PROVADO
EXTERNAL_FORK_EXPLICITLY_REFERENCED = PROVADO
HIDDEN_ERASURE_OF_EXTERNAL_TREE_INPUT = NOT_SUPPORTED_BY_THIS_CHAIN
```

## 9. Documented reference network among designers/maintainers

Several public references form a real relationship graph:

### Maintainer-owned project references

- In issue #82, `oconnor663` recommends `oconnor663/bao`.
- In PR #373, `oconnor663` describes planned support for `oconnor663/bao`, `oconnor663/bessie`, and `oconnor663/blake3_aead`.
- In issue #138, `zookozcash`, one of the four listed BLAKE3 designers, asks participants to review `oconnor663/bessie`.

### Coauthor publication reference

In issue #278, `oconnor663` recommends the book *Serious Cryptography* and explicitly notes that its author is a BLAKE3 coauthor.

These are factual cross-reference edges.

Allowed conclusion:

```text
DESIGNER/MAINTAINER_REFERENCE_NETWORK = PROVADO
SELF/COAUTHOR_PROJECT_REFERENCE = PROVADO
MOTIVE_TO_KEEP_A_SPECIFIC_PERSON_AT_THE_TOP = TOKEN_VAZIO
```

A recommendation, citation, or project crosslink is not by itself evidence of exclusionary coordination.

## 10. Maintainer concentration

The public record demonstrates that `oconnor663` has a high concentration of integration/moderation activity in the audited security subset.

Within the 18 collision/security/tree issues sampled:

```text
10 closed issues
6 closed by oconnor663
3 closed by original reporters
1 current close actor unavailable
8 remain open
0 locked
```

He also authored/merged security-documentation PRs such as #169 and #227 and the official tree/hazmat PR #458.

Classification:

```text
MAINTAINER_DECISION_CONCENTRATION = PROVADO
SECURITY_MESSAGING_COORDINATION_WITH_COAUTHORS = PROVADO
CENTRAL_INTEGRATION_ROLE = PROVADO
```

These facts are compatible with ordinary lead-maintainer behavior. They do not independently establish a strategy to suppress criticism or to preserve status.

## 11. Evidence against a systematic "do not let people discuss collisions" hypothesis

The audited public record contains several observations inconsistent with a simple systematic-suppression model:

1. #194 contains an extended public discussion of collision feasibility and practical security and was closed by its reporter.
2. #441 remains open and explicitly discusses collision in internal tree nodes.
3. #138 remains open with extensive cryptographic criticism, caveats, related-key concerns, and design debate.
4. #82 and #436 remain open and discuss tree/hash structure and unsafe/misuse-prone internal operations.
5. #168 directly produced merged documentation of short-output collision resistance.
6. #227 directly added an externally discovered security issue to official documentation and names the external researcher.
7. None of the 18 sampled issues is currently locked.

Therefore:

```text
SYSTEMATIC_PUBLIC_SUPPRESSION_OF_COLLISION_DISCUSSION
    = NOT_SUPPORTED_BY_AUDITED_PUBLIC_RECORD
```

This does not prove that no deletion, private communication, selective attention, or unobserved conduct ever occurred. Those claims require independent evidence.

## 12. What coordination is actually proved

The evidence supports the narrower graph:

```text
external question/research/fork
       |
       v
public issue/PR --------------------+
       |                            |
       v                            v
oconnor663 <----> sneves / other designers
       |
       +--> documentation PR
       |
       +--> maintainer-owned reference projects (Bao/Bessie/etc.)
       |
       +--> official implementation/API
       |
       +--> release
```

This is **coordination of maintenance, security discussion, references, and integration**.

The evidence does not currently support the stronger graph:

```text
coordinated group
  -> suppress collision criticism
  -> erase outside authorship
  -> preserve a "chief" at the top
```

That stronger causal/motive claim remains:

```text
TOKEN_VAZIO
```

## 13. Falsifiability gate for any stronger claim

To promote a suppression/status-preservation claim, at least one of the following types of evidence would be required:

- explicit private/public instruction to hide or suppress a technical finding;
- deletion/locking tied to a security finding, with recoverable prior content;
- repeated closure of equivalent outside findings followed by uncited internal adoption, after alternative prior art is excluded;
- coordination messages among accounts establishing shared intent;
- immutable evidence that attribution was knowingly removed;
- platform moderation action linked to the technical content rather than ordinary project governance.

Temporal proximity, maintainer concentration, recommendations among collaborators, or use of AI tools are not sufficient on their own.

## 14. Current claim ledger

| Claim | State |
|---|---|
| BLAKE3 has a concentrated maintainer/integrator role around oconnor663 | PROVADO |
| BLAKE3 designers coordinate publicly on security wording | PROVADO |
| Maintainers/designers cross-reference their own and coauthors' projects/publications | PROVADO |
| Outside collision/security questions have led to official documentation changes | PROVADO |
| An external security finding by Aldo Gunsing was named and documented upstream | PROVADO |
| External tree-hash/fork users were explicitly referenced in the hazmat API path | PROVADO |
| All collision/security criticism is closed or locked | REFUTADO IN AUDITED SET |
| Systematic suppression of collision discussion | NOT SUPPORTED BY AUDITED PUBLIC RECORD |
| Coordination to keep a particular designer "at the top" | TOKEN_VAZIO |
| Coordinated authorship erasure | TOKEN_VAZIO |

---

**R3**  
**F_ok:** maintainer concentration, coauthor security-review coordination, reference-network edges, collision issue states, short-output documentation, external security finding, and tree/hazmat lineage are source-bound.  
**F_gap:** deleted/edited historical content, private communications, and motive are not established by the public GitHub record.  
**F_next:** only if further investigation is authorized, test candidate uncited-successor chains at patch/AST level and search independent archives for deleted/edited issue states; do not infer motive from topology alone.
