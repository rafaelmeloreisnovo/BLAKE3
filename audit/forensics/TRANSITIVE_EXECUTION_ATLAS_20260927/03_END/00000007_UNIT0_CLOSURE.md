# μTRACE 0x00000007 — END / forensic unit 0 closure

parent_mu=0x00000006
phase=03_END
unit=0
evidence_state=PARTIAL_BUT_PRESERVED
claim_allowed=false
source_mutation=false

## Unit-0 result
The first bottom-up pass establishes a real transitive execution surface:
1. commit/push triggers CI on all branches;
2. workflow calls scripts/actions/package managers;
3. RMR scripts source common.sh;
4. common.sh performs external Git clone/fetch to a SHA-pinned upstream ref;
5. Cargo invokes build.rs and build dependencies;
6. build.rs invokes C/C++/ASM compilation through cc;
7. CMake contains conditional external FetchContent for SHA-pinned oneTBB;
8. CI contains remote-script-to-shell, moving action/tool refs, package-manager acquisition and an unpinned container digest;
9. root exact Cargo dependency resolution is not sealed by a root lockfile in the audited tree.

## Negative findings bounded to this pass
- Git submodules: not observed.
- executable symlink route: not observed; three observed symlinks are licenses.
- deliberate command encryption/encoding: not proven.
- malicious intent/control: not proven.
- indexed search negatives are explicitly non-authoritative.

## What remains open
TOKEN_VAZIO:
- complete internals and immutable identities of every third-party GitHub Action invocation;
- exact package versions delivered by apt/brew/cargo-install in each historical run;
- exact container image digest used by cargo-xwin historical runs;
- full crate-source/build.rs/proc-macro closure for every Cargo invocation;
- environment values at each historical workflow run;
- dynamic loader/shared-library closure of produced host binaries;
- historical change genealogy for each external edge;
- whether any environment/repository setting outside committed files altered these defaults.

## Directional atlas
0x00000000 -> 0x00000001 -> 0x00000002 -> 0x00000003 -> 0x00000004 -> 0x00000005 -> 0x00000006 -> 0x00000007

R3=<F_ok: first grain-level acquisition and transitive route evidence committed step-by-step; F_gap: external/action/package/runtime closure remains TOKEN_VAZIO; F_next: unit 1 expands each third-party action and historical execution identity>

This END closes only forensic unit 0. It is not a final repository verdict and not a final commit replacing intermediate evidence.
