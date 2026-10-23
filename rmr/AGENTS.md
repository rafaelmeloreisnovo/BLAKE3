# AGENTS.md — RMR byte-integrity / build observability

Scope: `rmr/**`. Inherit the repository-root AGENTS.md; more-specific child rules stay authoritative for their own directories.

## Non-negotiable source boundary
- Treat each tracked original file as byte-level evidence, including spelling, ASCII/UTF-8, comments, headers, timestamps written into source, whitespace, line endings, ordering, and names.
- NEVER apply formatters, newline conversion, license/header rewrite, mass refactors, or source edits merely to test optimization or to satisfy lint.
- Preserve the historical Git blob SHA. Perform experiments in separate scratch objects or new additive files; never mutate the pinned input.
- Git blob SHA means source-byte identity, **not** binary-byte identity. A source-byte change is important to custody even when it does not change machine code.
- Existing BLAKE3 upstream remains separately licensed and unmodified. Do not attribute upstream implementations to RMR.
- Never rewrite original documents or historic receipts as an audit shortcut.

## Mechanical build gate
- Record exact commit, Git blob/source manifest hash, path/case, flags, compiler/assembler/linker identity, target triple, entrypoint, section/link flags, and output SHA-256.
- Compare **the same ISA / ABI / flags / toolchain / environment** across two builds before claiming bit-identical output.
- Audit ELF program headers, dynamic needs, undefined symbols, sections, relocations, symbols, and file size. Absence of PT_INTERP / DT_NEEDED is not by itself evidence of bare-metal hardware execution.
- Source `void`, comments, warnings, aliases, dates, and names are audit dimensions. Ordinary comments are not linker directives. Effects on an artifact require a demonstrated build mechanism, including `__LINE__`, debug paths, generated text, or preprocessing where applicable.
- A successful static analyzer / cross-link / KAT / CI run NEVER implies physical ARM execution, independent reproduction, statistical speedup, cryptographic novelty, or release readiness.
- Record negative evidence and state explicitly: NOT_RUN / BLOCKED / TOKEN_VAZIO. Do not silence warnings to manufacture a pass.

## Write policy
- Refactor production code only in a separately authorized, reviewable change with source-original proof, before/after artifacts, KATs, measurements, and rollback.
- For the 2026-10-22 baseline study, allow only **additive** auditor/script/workflow/docs files; do not change `rmr/portable/**`, `rmr/fixed256/**`, `rmr/freestanding_custody16/**`, or any upstream file.
- Never auto-merge or promote. Require independent review and provider enforcement as separate gates.

## Read/order
1. `rmr/docs/00_START_HERE_RMR.md`
2. `rmr/docs/RMR_BUILD_AND_BINARY_CONTRACT_V1.md`
3. Code, assembly and build scripts pinned to exact SHA
4. `rmr/audit/rmr_binary_identity_v1.sh` when present
5. Receipts and negative tests, scoped to matching toolchain, target and HEAD.
