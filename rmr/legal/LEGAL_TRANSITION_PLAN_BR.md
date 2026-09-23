<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Documentation draft; does not modify rmr/LICENSE_RMR.
-->

# RMR legal transition plan — Brazil

State: DRAFT_FOR_COUNSEL_REVIEW
Effective license change: NOT_ACTIVE

## 1. Existing grant

The current `rmr/LICENSE_RMR` is permissive and expressly includes rights to use, copy, modify, merge, publish, distribute, sublicense and sell.

A later policy cannot honestly label previously distributed copies as if those rights had never been granted.

## 2. Prospective model

For RMR-authored material first published after an explicit future effective commit/date, the intended model may be:

1. Research license — no license fee for qualifying non-commercial research, teaching, reproducibility and independent validation.
2. Commercial license — separate written authorization required for commercial exploitation.
3. Enterprise/OEM license — negotiated support, redistribution, embedding, warranty and indemnity terms.
4. Upstream boundary — BLAKE3 and all third-party components retain their original licenses.

The final definitions of commercial, research, affiliate, revenue, distribution and derived work require counsel review because ambiguity increases enforcement risk.

## 3. Brazil legal anchors

The Brazilian Software Law (Lei 9.609/1998) protects computer programs under a copyright-like regime, states that protection does not depend on registration, and provides for license contracts. It also provides civil measures to stop infringement and claim damages.

The Copyright Law (Lei 9.610/1998) treats software under its specific law and protects the expression of works; ideas, methods and mathematical concepts as such are not protected by copyright.

The Civil Code provisions on contractual penalties require an actual contractual obligation. A penalty may not exceed the principal obligation and a judge may reduce it if manifestly excessive. Supplementary damages beyond the contractual penalty require an express agreement and proof of the excess.

## 4. What NOT to claim

- that a repository notice automatically creates a fine against every visitor;
- that all legal/audit costs are automatically payable by any alleged infringer;
- that an RMR license can override upstream BLAKE3 licenses;
- that changing the current license retroactively cancels prior permissions;
- that software registration is required for copyright protection.

## 5. Activation gate

Before changing `rmr/LICENSE_RMR`:

- freeze an exact inventory of RMR-authored files;
- identify all contributors and third-party dependencies;
- choose prospective effective commit/date;
- obtain Brazilian IP/contract counsel review;
- decide whether old files remain under the permissive grant or a clean-version boundary is necessary;
- publish a license matrix by path;
- obtain explicit assent for commercial contracts containing penalty/audit clauses;
- retain signed contract/version hashes.

Until this gate is complete:

~~~text
RESEARCH_COMMERCIAL_V2 = DRAFT
COMMERCIAL_RESTRICTION = NOT_ACTIVE
CONTRACTUAL_PENALTY = TOKEN_VAZIO_COUNSEL_AND_ASSENT
~~~
