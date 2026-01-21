# Relatório de Auditoria do Fork BLAKE3

## Escopo

Este relatório identifica **todas** as diferenças em relação ao upstream
oficial do BLAKE3, classifica camadas (núcleo vs externo), lista operações
adicionadas pelo fork e enumera opções de build/features.

## Diferenças vs upstream (sumário)

```
.github/workflows/ci.yml                 |    2 +-
 AGENTS.md                                |   16 +
 Cargo.toml                               |   19 +-
 DOCUMENTACAO.md                          |   85 +
 FORK_NOTES.md                            |   53 +
 MANIFESTO.md                             |  102 +
 MANIFESTO_RAFAELIA.md                    |   70 +
 README.md                                |   18 +
 RELATORIO_AUDITORIA.md                   |  292 ++
 audit/diff_bindings.patch                |    0
 audit/diff_c.patch                       |  315 ++
 audit/diff_name_status.txt               |  107 +
 audit/diff_python.patch                  |    0
 audit/diff_rmr.patch                     | 5008 ++++++++++++++++++++++++++++++
 audit/diff_scripts.patch                 |    0
 audit/diff_src.patch                     |  656 ++++
 audit/diff_stat.txt                      |  108 +
 audit/diff_tools.patch                   |   60 +
 b3sum/Cargo.lock                         |  282 +-
 b3sum/Cargo.toml                         |    4 +-
 b3sum/src/main.rs                        |   39 +-
 c/CMakeLists.txt                         |   31 +-
 c/blake3.h                               |   30 +-
 c/blake3_c_rust_bindings/Cargo.toml      |    2 +-
 c/blake3_c_rust_bindings/src/lib.rs      |    6 +-
 c/blake3_dispatch.c                      |    4 +-
 c/blake3_impl.h                          |    2 +-
 c/blake3_neon.c                          |    9 +-
 c/blake3_neon_aarch64_unix.S             |   19 +
 c/blake3_neon_armv7_unix.S               |   19 +
 c/test.py                                |    9 +-
 reference_impl/Cargo.toml                |    2 +-
 rmr/ARCHITECTURE.md                      |   39 +
 rmr/LICENSE_RMR                          |   21 +
 rmr/PRIVACY.md                           |   30 +
 rmr/PROVENIENCE.md                       |   39 +
 rmr/REVIEW.md                            |   38 +
 rmr/base.c                               |  199 ++
 rmr/base.o                               |  Bin 0 -> 7760 bytes
 rmr/bench.c                              |  184 ++
 rmr/bench.o                              |  Bin 0 -> 7624 bytes
 rmr/bench_termux.sh                      |   79 +
 rmr/benchdiff.c                          |  121 +
 rmr/benchdiff.o                          |  Bin 0 -> 5968 bytes
 rmr/build_termux.sh                      |    6 +
 rmr/cli.c                                |   78 +
 rmr/cli.o                                |  Bin 0 -> 6312 bytes
 rmr/fix_geom_N_all_scopes.py             |  100 +
 rmr/fix_geom_N_scope.py                  |   89 +
 rmr/fix_pai_warnings.py                  |  167 +
 rmr/geom.c                               |  312 ++
 rmr/geom.c.bak                           |  304 ++
 rmr/geom.c.pre_fix.1768878317.bak        |  310 ++
 rmr/geom.c.pre_fix.1768917982.bak        |  308 ++
 rmr/geom.o                               |  Bin 0 -> 10496 bytes
 rmr/geom.pre_fix.1768918156.bak          |  308 ++
 rmr/geom.pre_fix.1768918259.bak          |  308 ++
 rmr/hash_sha256.c                        |  123 +
 rmr/hash_sha256.c.bak                    |  122 +
 rmr/hash_sha256.c.pre_warnfix.bak        |  123 +
 rmr/hash_sha256.o                        |  Bin 0 -> 4464 bytes
 rmr/include/rmr_arch.h                   |   67 +
 rmr/include/rmr_dispatch.h               |   57 +
 rmr/include/rmr_lowlevel.h               |  207 ++
 rmr/main.c                               |    5 +
 rmr/main.o                               |  Bin 0 -> 1048 bytes
 rmr/pai.h                                |   26 +
 rmr/pai_base.h                           |    8 +
 rmr/pai_bench.h                          |    6 +
 rmr/pai_benchdiff.h                      |    6 +
 rmr/pai_geom.h                           |    4 +
 rmr/pai_hash.h                           |   18 +
 rmr/pai_scan.h                           |   19 +
 rmr/pai_sign.h                           |    6 +
 rmr/pai_toroid.h                         |    4 +
 rmr/rust/rmr.rs                          |   28 +
 rmr/scan.c                               |  278 ++
 rmr/scan.o                               |  Bin 0 -> 10512 bytes
 rmr/sign.c                               |  121 +
 rmr/sign.o                               |  Bin 0 -> 4264 bytes
 rmr/toroid.c                             |  148 +
 rmr/toroid.c.bak                         |  128 +
 rmr/toroid.c.pre_warnfix.bak             |  128 +
 rmr/toroid.o                             |  Bin 0 -> 6088 bytes
 rmr/util.c                               |   34 +
 rmr/util.o                               |  Bin 0 -> 2464 bytes
 src/ffi_avx2.rs                          |    3 +-
 src/ffi_avx512.rs                        |    6 +-
 src/ffi_neon.rs                          |    5 +-
 src/ffi_sse2.rs                          |    5 +-
 src/ffi_sse41.rs                         |    5 +-
 src/guts.rs                              |    3 +
 src/hazmat.rs                            |    4 +
 src/join.rs                              |    4 +-
 src/lib.rs                               |   22 +-
 src/platform.rs                          |   11 +
 src/portable.rs                          |    4 +
 src/rust_avx2.rs                         |    2 +
 src/rust_sse2.rs                         |    5 +
 src/rust_sse41.rs                        |    5 +
 src/traits.rs                            |   16 +-
 src/wasm32_simd.rs                       |    5 +
 test_vectors/Cargo.toml                  |    2 +-
 tools/benchmark_example.sh               |   16 +
 tools/compiler_version/Cargo.toml        |    2 +-
 tools/instruction_set_support/Cargo.toml |    2 +-
 tools/release.md                         |    2 +-
 107 files changed, 11936 insertions(+), 268 deletions(-)
```

## Auditoria de camadas

- **Verificação de fronteira (`c/` e `src/`)**: não foi encontrado nenhum
  `rmr_*.h` nem includes externos dentro do núcleo (`rg -n "rmr_" c src`).
- **Proposta de layout**: manter o núcleo BLAKE3 intacto em `c/`, `src/`,
  `reference_impl/`, `b3sum/`, `test_vectors/`, `tools/` e mover qualquer
  extensão para `rmr/`.

## Operações adicionadas pelo fork (camada externa RMR)

- **CLI RMR**: `rmr/cli.c` + `rmr/main.c`.
- **Benchmarks**: `rmr/bench.c`, `rmr/benchdiff.c`, scripts `rmr/bench_termux.sh`.
- **Build Termux**: `rmr/build_termux.sh`.
- **Scan/Sign**: `rmr/scan.c`, `rmr/sign.c`.
- **Bases auxiliares**: `rmr/base.c`, `rmr/util.c`, `rmr/geom.c`, `rmr/toroid.c`.
- **SHA-256 auxiliar**: `rmr/hash_sha256.c`.
- **Headers de interface**: `rmr/pai_*.h`.
- **Scripts de ajuste**: `rmr/fix_*.py`.

## Opções e flags disponíveis

### Cargo features (Rust)

- `std` (default)
- `rayon`
- `mmap`
- `zeroize`
- `serde`
- `traits-preview`
- `digest`
- `pure`
- `prefer_intrinsics`
- `no_sse2`, `no_sse41`, `no_avx2`, `no_avx512`, `no_neon`

### CMake flags (C)

- `BLAKE3_USE_TBB`
- `BLAKE3_FETCH_TBB`
- `BLAKE3_SIMD_TYPE`
- `BLAKE3_CFLAGS_SSE2`, `BLAKE3_CFLAGS_SSE4.1`, `BLAKE3_CFLAGS_AVX2`,
  `BLAKE3_CFLAGS_AVX512`, `BLAKE3_CFLAGS_NEON`

### Scripts SH

- `c/blake3_c_rust_bindings/cross_test.sh`
- `test_vectors/cross_test.sh`
- `tools/benchmark_example.sh`
- `rmr/build_termux.sh`
- `rmr/bench_termux.sh`

### Flags RMR (externos)

- `RMR_COMPILER_*` (clang/gcc/msvc)
- `RMR_OS_*` (windows/darwin/linux/android/freebsd/openbsd/netbsd)
- `RMR_ARCH_*` (x86_64/x86_32/aarch64/arm/ppc/riscv)
- `RMR_LITTLE_ENDIAN` / `RMR_BIG_ENDIAN`
- `RMR_ASM_INTEL` / `RMR_ASM_ARM`
- `RMR_HAS_*` (sse2/sse41/avx2/avx512/neon)
- `RMR_NO_LIBC` (modo sem libc)

## Relatório completo (arquivo por arquivo)

| Caminho | Tipo | Camada | Mudança | Função/impacto |
| --- | --- | --- | --- | --- |
| `.github/workflows/ci.yml` | YAML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `AGENTS.md` | MD | EXTERNO (fork) | Add | Contrato operacional do repositório. |
| `Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `DOCUMENTACAO.md` | MD | EXTERNO (fork) | Add | Documentação do fork e separação de camadas. |
| `FORK_NOTES.md` | MD | EXTERNO (fork) | Add | Notas do fork, diferenças e build Termux. |
| `MANIFESTO.md` | MD | EXTERNO (fork) | Add | Manifesto autoral do fork (camada externa). |
| `MANIFESTO_RAFAELIA.md` | MD | EXTERNO (fork) | Add | Manifesto autoral complementar do fork. |
| `README.md` | MD | NÚCLEO (upstream) | Mod | README upstream com seções específicas do fork. |
| `RELATORIO_AUDITORIA.md` | MD | EXTERNO (auditoria) | Add | Relatório completo de diferenças e camadas. |
| `audit/diff_bindings.patch` | PATCH | EXTERNO (auditoria) | Add | Diff do diretório bindings/ (quando existir). |
| `audit/diff_c.patch` | PATCH | EXTERNO (auditoria) | Add | Diff completo do diretório c/ vs upstream. |
| `audit/diff_name_status.txt` | TXT | EXTERNO (auditoria) | Add | Registro de diff name-status vs upstream. |
| `audit/diff_python.patch` | PATCH | EXTERNO (auditoria) | Add | Diff do diretório python/ (quando existir). |
| `audit/diff_rmr.patch` | PATCH | EXTERNO (auditoria) | Add | Diff do diretório rmr/ vs upstream. |
| `audit/diff_scripts.patch` | PATCH | EXTERNO (auditoria) | Add | Diff do diretório scripts/ (quando existir). |
| `audit/diff_src.patch` | PATCH | EXTERNO (auditoria) | Add | Diff completo do diretório src/ vs upstream. |
| `audit/diff_stat.txt` | TXT | EXTERNO (auditoria) | Add | Registro de diff --stat vs upstream. |
| `audit/diff_tools.patch` | PATCH | EXTERNO (auditoria) | Add | Diff do diretório tools/ vs upstream. |
| `b3sum/Cargo.lock` | LOCK | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `b3sum/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `b3sum/src/main.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/CMakeLists.txt` | TXT | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3.h` | H | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_c_rust_bindings/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_c_rust_bindings/src/lib.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_dispatch.c` | C | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_impl.h` | H | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_neon.c` | C | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `c/blake3_neon_aarch64_unix.S` | ASM | NÚCLEO (upstream) | Add | Diferença registrada em relação ao upstream. |
| `c/blake3_neon_armv7_unix.S` | ASM | NÚCLEO (upstream) | Add | Diferença registrada em relação ao upstream. |
| `c/test.py` | PY | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `reference_impl/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `rmr/ARCHITECTURE.md` | MD | EXTERNO (RMR) | Add | Arquitetura da camada externa RMR. |
| `rmr/LICENSE_RMR` | OTHER | EXTERNO (RMR) | Add | Licença do módulo RMR. |
| `rmr/PRIVACY.md` | MD | EXTERNO (RMR) | Add | Política de privacidade do RMR. |
| `rmr/PROVENIENCE.md` | MD | EXTERNO (RMR) | Add | Mapa de proveniência da camada externa. |
| `rmr/REVIEW.md` | MD | EXTERNO (RMR) | Add | Checklist de revisão/determinismo do RMR. |
| `rmr/base.c` | C | EXTERNO (RMR) | Add | Base utilitária do RMR. |
| `rmr/base.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/bench.c` | C | EXTERNO (RMR) | Add | Benchmark RMR (camada externa). |
| `rmr/bench.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/bench_termux.sh` | SH | EXTERNO (RMR) | Add | Script de benchmark no Termux. |
| `rmr/benchdiff.c` | C | EXTERNO (RMR) | Add | Benchmark comparativo RMR. |
| `rmr/benchdiff.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/build_termux.sh` | SH | EXTERNO (RMR) | Add | Script de build no Termux. |
| `rmr/cli.c` | C | EXTERNO (RMR) | Add | CLI do módulo RMR. |
| `rmr/cli.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/fix_geom_N_all_scopes.py` | PY | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/fix_geom_N_scope.py` | PY | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/fix_pai_warnings.py` | PY | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/geom.c` | C | EXTERNO (RMR) | Add | Rotinas geométricas do RMR. |
| `rmr/geom.c.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/geom.c.pre_fix.1768878317.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/geom.c.pre_fix.1768917982.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/geom.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/geom.pre_fix.1768918156.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/geom.pre_fix.1768918259.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/hash_sha256.c` | C | EXTERNO (RMR) | Add | Implementação SHA-256 auxiliar do RMR. |
| `rmr/hash_sha256.c.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/hash_sha256.c.pre_warnfix.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/hash_sha256.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/include/rmr_arch.h` | H | EXTERNO (RMR) | Add | Detecção de arquitetura/OS para camada externa. |
| `rmr/include/rmr_dispatch.h` | H | EXTERNO (RMR) | Add | Flags de dispatch/feature do RMR. |
| `rmr/include/rmr_lowlevel.h` | H | EXTERNO (RMR) | Add | Helpers low-level opcionais do RMR. |
| `rmr/main.c` | C | EXTERNO (RMR) | Add | Ponto de entrada do executável RMR. |
| `rmr/main.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/pai.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_base.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_bench.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_benchdiff.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_geom.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_hash.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_scan.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_sign.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/pai_toroid.h` | H | EXTERNO (RMR) | Add | Diferença registrada em relação ao upstream. |
| `rmr/rust/rmr.rs` | RS | EXTERNO (RMR) | Add | Módulo Rust externo do RMR. |
| `rmr/scan.c` | C | EXTERNO (RMR) | Add | Rotinas de scan RMR. |
| `rmr/scan.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/sign.c` | C | EXTERNO (RMR) | Add | Rotinas de assinatura RMR. |
| `rmr/sign.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/toroid.c` | C | EXTERNO (RMR) | Add | Rotinas toroidais do RMR. |
| `rmr/toroid.c.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/toroid.c.pre_warnfix.bak` | BACKUP | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/toroid.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `rmr/util.c` | C | EXTERNO (RMR) | Add | Utilidades do RMR. |
| `rmr/util.o` | BIN | EXTERNO (RMR) | Add | Artefato de build/backup do RMR (não pertence ao núcleo). |
| `src/ffi_avx2.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/ffi_avx512.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/ffi_neon.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/ffi_sse2.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/ffi_sse41.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/guts.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/hazmat.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/join.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/lib.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/platform.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/portable.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/rust_avx2.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/rust_sse2.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/rust_sse41.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/traits.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `src/wasm32_simd.rs` | RS | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `test_vectors/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `tools/benchmark_example.sh` | SH | NÚCLEO (upstream) | Add | Script de benchmark do upstream. |
| `tools/compiler_version/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `tools/instruction_set_support/Cargo.toml` | TOML | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
| `tools/release.md` | MD | NÚCLEO (upstream) | Mod | Diferença registrada em relação ao upstream. |
