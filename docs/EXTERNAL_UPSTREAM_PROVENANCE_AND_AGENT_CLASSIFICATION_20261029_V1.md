# External upstream and agent provenance — 2026-10-29 V1
Status: DRAFT/HOLD; no upstream merge; no clone or build; no rights promotion.
Companion branch: `refactor/rmr-authorial-core-20261029-draft`.
Parent baseline: `rafaelmeloreisnovo/BLAKE3@718a8f8e200c42aecfdf5ffedcab6d130ed3dcfb`.
Guard: SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM. Unknown `TOKEN_VAZIO` != 0.

## A. Provenance boundary for the BLAKE3 repository
- Original upstream: https://github.com/BLAKE3-team/BLAKE3; algorithm, `c/`, `src/`, `b3sum/`, vector suites and licensing remain upstream.
- Derivative RMR layer: https://github.com/rafaelmeloreisnovo/BLAKE3 and `rmr/`; candidate project-owned additions **not** globally adjudicated as authorial without file-by-file review.
- Upstream official fix 2026-10-01: `f55849f89cd85083c1c9daa2c5de20766f309c00`. Fork observed HEAD 2026-10-27: `718a8f8e200c42aecfdf5ffedcab6d130ed3dcfb`. `c/blake3.c` and `src/lib.rs` were identical Git blobs at these refs, `c/blake3_avx512_x86-64_unix.S` and `src/test.rs` differed. This is a targeted source gap, not proven runtime vulnerability.
- No upstream C/ASM/Rust source or copyright header may be rewritten during refactor.

## B. Producer/integration candidates (no inferred copying, license or influence)
| Project / organization | Authoritative source to inspect | Function relevant to branch | Proposed class | Evidence-gate |
| --- | --- | --- | --- | --- |
| ZIPRAF_CORE | https://github.com/rafaelmeloreisnovo/ZIPRAF_CORE | C ZRF codec, compression and traceability | PROJECT_SEPARATE_PRODUCER | `TOKEN_VAZIO`: exact API/commit KAT/build |
| ZIPRAF_OMEGA_FULL | https://github.com/rafaelmeloreisnovo/ZIPRAF_OMEGA_FULL | Python Bitraf64 prototypes; legacy RLE ambiguity documented | PROJECT_SEPARATE_PRODUCER | `TOKEN_VAZIO`: v1.4/RLE2 gate and full-overhead compression |
| Templo Vivo ARCΣ | https://github.com/rafaelmeloreisnovo/templo-vivo-arcs | Flutter/Android and ZIP history 2025-03 | PROJECT_SEPARATE_SOURCE | `TOKEN_VAZIO`: original 2005/2012 dated artifacts, legal lineage |
| MyCat | https://github.com/rafaelmeloreisnovo/myCat-iahelpsus | Fork based on https://github.com/yumiaura/mycat | THIRD_PARTY_FORK | Recorded merge-base e9fba63...; no upstream license found in scoped audits; follow-up mandatory |
| IBM contribution → Eclipse OpenJ9 | https://github.com/eclipse-openj9/openj9 | Java JVM/VM/runtime reference | EXTERNAL_REFERENCE | version, license and cross-repo byte comparison required |
| Microsoft Florence-2 | https://huggingface.co/microsoft/Florence-2-base | Image/video-language model reference **if requested Florence means the model** | EXTERNAL_MODEL_REFERENCE | model card / weights/license and input/output provenance |
| Florence on-screen keyboard | https://github.com/rafaelmeloreisnovo/termux-packages/blob/master/x11-packages/florence/build.sh | Separate on-screen keyboard; *not* Florence-2 by identity | EXTERNAL_PACKAGE_REFERENCE | clarify which Florence + pin source |
| Google AndroidX | https://android.googlesource.com/platform/frameworks/support/ | Lifecycle, app UI, storage/JVM adapters | EXTERNAL_HOSTED_PLATFORM | AndroidX Maven version and license manifest |
| Android/QEMU own work | https://github.com/rafaelmeloreisnovo/qemu_rafaelia ; https://github.com/rafaelmeloreisnovo/Vectras-VM-Android | QEMU shell, QMP, JNI, Android integration | PROJECT_FORK_ADAPTER | exact upstream diff, package/ABI checks, source-owner rights |
| QEMU official | https://gitlab.com/qemu-project/qemu | Emulator/TCG execution source | EXTERNAL_UPSTREAM | preserved upstream copyright, SPDX and diff |
| Java / OpenJDK | https://github.com/openjdk/jdk | Java language/runtime library reference | EXTERNAL_REFERENCE | exact JDK commit/license and implemented boundary |
| Oracle GraalVM | https://github.com/oracle/graal | AOT/JIT/compiler/Native Image reference | EXTERNAL_REFERENCE | precise component license (varies) / JNI boundary |

### Repositories already containing related work
- https://github.com/rafaelmeloreisnovo/termux-app-rafacodephi/blob/master/docs/audits/QEMU_ANDROIDX_RMR_CORRECTION_AUDIT.md — describes QEMU upstream core preserved and RAFAELIA integration hooks; historical report not current HEAD runtime validation.
- https://github.com/rafaelmeloreisnovo/Vectras-VM-Android/blob/master/docs/INTEGRACAO_RM_QEMU_ANDROIDX.md — adapter strategy across QMP, AndroidX and Vectras; source paths require exact current commit pinning.
- https://github.com/rafaelmeloreisnovo/termux-app-rafacodephi/blob/master/docs/audits/FLORENCE_FILESYSTEM_HARDWARE_DIAGNOSTIC.md — **Florence named filesystem/hardware diagnostic**, may not be either Microsoft model or keyboard. Do not conflate names.
- Search found ZIPRAF source references in `rafaelmeloreisnovo/nanoGPT`, `UserLAnd`, `Rafaelia_Private`, `GAIA_phi`, `verbum-vivo`; lexical similarity is a discovery key, not provenance proof.

## C. MyCat bot/AI classification
Source: https://github.com/rafaelmeloreisnovo/myCat-iahelpsus/blob/main/provenance/upstream_snapshot_2026-08-01.json
- **VERIFIED DOCUMENTED**: fork from yumiaura/myCat; historic merge-base `e9fba63be25bfd61cf0518016fd63e5a4b4ad964`, CI/test/release automation recorded.
- **NOT_VERIFIED**: owner account is a fully autonomous bot, personal AI moved to platform, or follow/unfollow was automatically caused by fork/push.
- Human + workflow automation + optional AI assistance **cannot** be distinguished by absence of local repository, username style or CI YAML alone.
- Classification fields proposed for future evidence: `provider_account_type`, `actor_login`, `event_type`, `event_time_utc`, `bot_signature_or_app_slug`, `workflow_run_id`, `commit_signature`, `manual_action_evidence`, `claim_gate`; unknown stays `TOKEN_VAZIO`.
- License warning: prior audit found no explicit upstream license, so copied code/art remains review-blocked.

## D. Wayback provenance / host variants
Targets: cesarreis.com.br and reisoffice.com.br; each apex/www × HTTP/HTTPS, plus portal/mobile when archive material is available.
Historical text replay accessed: https://web.archive.org/web/20051215052151/http://www.cesarreis.com.br/ . A timestamp in the Wayback URL is a **capture time**, not a source publication or original-code date.
CDX source fields to preserve: `timestamp, original, statuscode, mimetype, digest, length, WARC location`. CDX digest is not a signature of authorship or proof of tampering.
- `TOKEN_VAZIO_RAW_WARC`: current audit not a complete byte-level archive reconstruction.
- `TOKEN_VAZIO_COMMON_IIS`: same `physicalPath` and bindings not demonstrated.
- `TOKEN_VAZIO_DEPLOY_GIT`: no build/commit-to-host deployment proof.
- `TOKEN_VAZIO_COLLUSION`: no evidence of coordinated alteration established.

## E. Minimal controls before promoting a reference into the candidate authorial core
1. Obtain source and exact upstream commit/hash, license and provenance.
2. Separate interface/specification from protected implementation; no blind copy or automated upstream sync.
3. Pin compile/runtime dependency as opposed to reference-only relationship.
4. Run differential tests and source/ELF/ABI gates on source and candidate replacements.
5. Prove rollback and preserve both branches unmerged.
6. Preserve failures, gaps, author rights and private prompt boundaries as typed `TOKEN_VAZIO`.

R3 = <F_ok: independent external-source branch + prior MyCat provenance and external repo references documented, F_gap: precise dependencies and bot autonomy unverified, F_next: pin provider versions and compare actual source bytes with licenses/tests; retain external reference isolation>.
