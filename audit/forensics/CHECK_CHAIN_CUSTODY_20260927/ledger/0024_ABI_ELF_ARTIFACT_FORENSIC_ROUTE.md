# μ0024 — ABI/ELF artifact forensic route

- date: 2026-09-27
- parent_commit: `9346454b0b8220c89e58e7f6a7431fa1d711e72b`
- audited_fork_commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`
- official_commit: `6aab490a26124663329dfd3961b8469f8fdb158b`
- workflow_run: `36287272177`
- artifact_id: `10921588899`
- artifact_name: `upstream-v3-abi-elf`
- kind: ABI_ELF_BINARY_BOUNDARY_FORENSICS
- completion_state: ARTIFACT_OBSERVED
- claim_allowed: false

## μ0_START — evidence package

The retained GitHub Actions artifact was downloaded and inspected as a ZIP.

Artifact payload includes:

- `SHA256SUMS.txt`;
- `summary.json`;
- environment receipt;
- official/fork ABI probes;
- official/fork symbol and undefined-symbol inventories;
- ELF program headers and section inventories;
- readelf symbol tables;
- GNU-stack receipts;
- size reports;
- build/configure logs;
- `abi.diff`.

Environment receipt binds:

- observed UTC: `2026-09-27T03:16:02Z`;
- official ref/commit: `6aab490a26124663329dfd3961b8469f8fdb158b`;
- fork commit: `b1c8cab1d75943a628e447fa72bb06ebeb348489`;
- host: Ubuntu x86_64 / Linux Azure runner;
- compiler: Ubuntu clang 18.1.3;
- CMake: 3.31.6;
- `claim_allowed=false`.

Boundary:

`ABI_EQUAL != SYMBOL_SET_EQUAL != BINARY_IDENTICAL != RUNTIME_DEPENDENCY_IDENTICAL`

## μ1_RESOLVE — binary facts

### A. public ABI

`summary.json` reports:

- `public_abi_probe_equal=true`;
- no `symbols_only_fork`;
- no `symbols_only_official`.

`abi.diff` is zero bytes and hashes to the SHA-256 of an empty file.

Official and fork ABI probe files share:

`sha256:94464bfa8b8cfdd47e8c8e88d2d39486b23fe5740fe1b1224258e2fcfedfef45`.

Observed ABI fields are equal, including:
- version 1.8.7;
- key length 32;
- output length 32;
- block length 64;
- chunk length 1024;
- chunk-state size 112;
- hasher size 1912;
- recorded member offsets.

Classification:

`PUBLIC_ABI_EQUAL = OBSERVED`.

### B. symbol-name closure

Fork and official exported/symbol inventories share:

`sha256:45569d50ad05d9449141e14a084deedd4e9d957fb361652183360e787896610b`.

Fork and official static-archive undefined-symbol inventories share:

`sha256:38df6b283d6475f22ee17a41e6609954ccc6ffaf02e0fc76d857c07f7f779575`.

The static archive's undefined set contains:
- cross-object BLAKE3 implementation symbols;
- `memcpy`;
- `strlen`.

These undefined entries are an archive-level linkage inventory. They do not mean the final executable retains all of them unresolved.

Classification:

`STATIC_ARCHIVE_SYMBOL_SET_EQUAL = OBSERVED`.

### C. final example ELF is dynamically linked

The fork example program headers identify:
- ELF type `DYN` / PIE;
- interpreter `/lib64/ld-linux-x86-64.so.2`;
- dynamic section / PLT / GOT / relocation structures;
- GNU stack marked read/write and non-executable.

The readelf symbol table contains versioned GLIBC imports including:
- `putchar@GLIBC_2.2.5`;
- `__libc_start_main@GLIBC_2.34`;
- `__errno_location@GLIBC_2.2.5`;
- `strlen@GLIBC_2.2.5`;
- `printf@GLIBC_2.2.5`;
- `read@GLIBC_2.2.5`;
- `fprintf@GLIBC_2.2.5`;
- `memcpy@GLIBC_2.14`;
- `strerror@GLIBC_2.2.5`;
- `stderr@GLIBC_2.2.5`;
- weak runtime symbols such as `__cxa_finalize` and instrumentation/transactional-memory placeholders.

Therefore the test example is not a freestanding/no-loader artifact.

This artifact does not include a separate `readelf -d` / `DT_NEEDED` inventory, so the complete dynamic-library list is not claimed from this package.

No OpenSSL symbol/import appears in this plain BLAKE3 example evidence.

### D. GNU stack

Both official and fork receipts report:

`gnu_stack=PASS_NON_EXECUTABLE`.

The stack receipt is byte-identical:

`sha256:aded3e79a07bb33fc5a87ddde18b971aaca518ef5fbb0011777f99a58f755ffc`.

### E. binary-size divergence despite ABI equality

Artifact summary:
- official example: 97,608 bytes;
- fork example: 97,608 bytes;
- official static library: 99,830 bytes;
- fork static library: 99,990 bytes.

Static-library delta:

`fork - official = +160 bytes`.

The ELF section-size report shows:
- official `.text`: 79,257 bytes;
- fork `.text`: 79,385 bytes;
- delta: `+128`;

- official `.eh_frame`: 1,368 bytes;
- fork `.eh_frame`: 1,416 bytes;
- delta: `+48`;

- official section total: 84,250 bytes;
- fork section total: 84,426 bytes;
- delta: `+176`.

Program-header executable LOAD file size differs by +0x80 (128 bytes) and the following read-only LOAD by +0x30 (48 bytes), matching the `.text` and `.eh_frame` deltas.

Relevant evidence hashes differ:
- fork program headers:
  `e390cd31f1aecc0b0b0cde9429affb9d9cd45544e53e26dcbff3b61fce37e4b2`;
- official program headers:
  `53c0ba1d8df99f756ffb21e79ad568de19384d702cff669950cba6cdc2fd12cb`;

- fork size:
  `43ef1d28d580c09aa5ad604213dce332299fb9480527605bdc920eecd16083a4`;
- official size:
  `948e295a547e26f4b6cc6268222ecbae71c15ea9287482d078b24b998dcefa3a`.

Classification:

`ABI_EQUAL = TRUE`

`SYMBOL_SET_EQUAL = TRUE`

`BINARY_LAYOUT_IDENTICAL = FALSE`

The cause of the size/code-layout delta is not assigned in this grain. Candidate causes include source/build/profile/codegen differences, but attribution requires controlled ablation evidence.

## μ2_EXECUTE_OBSERVE — loader and shadow interpretation

Observed execution/runtime boundaries:

1. standard ELF interpreter is present;
2. dynamic GLIBC imports are present;
3. PLT/GOT/dynamic relocation structures are present;
4. GNU stack is non-executable;
5. no extra public symbol appears only on the fork side;
6. no explicit evidence in this artifact of a custom hidden loader;
7. no OpenSSL linkage is present in the plain BLAKE3 example evidence;
8. fork code/layout differs from official despite matching ABI/symbol names.

Important distinctions:

`STANDARD_ELF_DYNAMIC_LOADER != HIDDEN_LOADER`

`SAME_SYMBOL_NAMES != SAME_MACHINE_CODE`

`SAME_OUTPUT_SIZE != SAME_BINARY_CONTENT`

`ARCHIVE_UNDEFINED_REFERENCE != FINAL_UNRESOLVED_FAILURE`

The reserved symbol-table index-zero `STN_UNDEF` entry also appears in the final readelf output. This is consistent with μ0006 and must not be classified as a named unresolved external symbol.

## μ3_CLOSE — custody result and next route

### Strongest result

The artifact proves that the fork and official comparison preserved the probed public ABI and symbol-name surface, while the compiled code/unwind layout and static archive sizes were not identical.

That is exactly the kind of lower-level distinction a source-only or check-name-only audit would miss.

### Missing evidence

- complete `DT_NEEDED` / RUNPATH/RPATH inventory;
- raw example/static-library binaries in this artifact for independent byte hashing/disassembly;
- instruction-level diff responsible for the +128-byte text delta;
- causal attribution of the code-size delta;
- RMR crypto-runtime ELF linkage to OpenSSL/provider modules.

### R3

- F_ok: ABI, symbol, loader, stack and binary-layout evidence reconstructed from retained artifact.
- F_gap: exact dynamic-library table and machine-code causal diff remain incomplete.
- F_next: inspect the retained RMR integration artifact for OpenSSL/runtime-provider, build and linkage receipts.
