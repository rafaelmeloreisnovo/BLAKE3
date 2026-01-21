# Relatório de Auditoria Total — Fork BLAKE3 vs Upstream

> **Status:** auditoria estática (sem execução de benchmarks). Onde não há
> evidência observável no código/arquivos, a marcação é **NÃO EVIDENCIADO**.

## 0) Inventário do Repo (Passo 0)

### 0.1 Árvore de diretórios (profundidade 2)

```
.
./.cargo
./.git
./.git/branches
./.git/hooks
./.git/info
./.git/logs
./.git/objects
./.git/refs
./.github
./.github/workflows
./audit
./b3sum
./b3sum/src
./b3sum/tests
./benches
./c
./c/blake3_c_rust_bindings
./c/cmake
./c/dependencies
./media
./reference_impl
./rmr
./rmr/include
./rmr/rust
./src
./test_vectors
./test_vectors/src
./tools
./tools/compiler_version
./tools/instruction_set_support
```

### 0.2 Lista de arquivos por tipo (contagem)

| Extensão | Contagem | Observações |
| --- | ---:| --- |
| `.rs` | 34 | Rust (core + bindings) |
| `.c` | 22 | C (core + RMR) |
| `.md` | 18 | Documentação |
| `.h` | 14 | Headers C |
| `.s` | 10 | ASM (core) |
| `.asm` | 4 | ASM adicional |
| `.sh` | 5 | Scripts de shell |
| `.toml` | 8 | Configs Rust |
| `.json` | 2 | Configs CMake/CI |
| `.py` | 4 | Scripts Python |
| `.yml`/`.yaml` | NÃO EVIDENCIADO | (não detectado no inventário) |
| `.o` | 11 | **Artefatos de build (RMR)** |
| `.bak` | 9 | **Artefatos de backup (RMR)** |
| `NOEXT` | 11 | Arquivos sem extensão |

> Observação: há artefatos `.o` e `.bak` dentro de `rmr/`, indicando material
> de build/backup externo, **não pertencente ao core**.

### 0.3 Zonas do repo (classificação)

- **CORE (upstream):** `src/`, `c/`, `b3sum/`, `reference_impl/`, `test_vectors/`,
  `benches/`, `tools/`, `media/`, `.github/`.
- **Bindings:** `c/blake3_c_rust_bindings/`.
- **Docs upstream:** `README.md`, `LICENSE_*`, `c/README.md`.
- **Camada externa (RMR/EXTERNAL):** `rmr/`, `DOCUMENTACAO.md`, `MANIFESTO*.md`,
  `RELATORIO_AUDITORIA.md`, `audit/`.

---

## 1) Diferenças vs Upstream (Passo 1)

**Referência upstream:** `upstream/master`.

### 1.1 Diff por arquivo (name-status)

> **Fonte:** `git diff --name-status upstream/master`.

**Tabela (auditável):**

| file | status | layer | impact | rationale | risk | notes |
| --- | --- | --- | --- | --- | --- | --- |
| `.github/workflows/ci.yml` | M | CORE | CI | Pipeline de CI | Médio | CI mudou vs upstream |
| `AGENTS.md` | A | RMR/EXTERNAL | docs | Instruções operacionais | Baixo | Doc local |
| `Cargo.toml` | M | CORE | build | Configuração do crate | Médio | Altera build |
| `DOCUMENTACAO.md` | A | RMR/EXTERNAL | docs | Documentação externa | Baixo | Isolado |
| `FORK_NOTES.md` | A | RMR/EXTERNAL | docs | Notas do fork | Baixo | Isolado |
| `MANIFESTO.md` | A | RMR/EXTERNAL | docs | Manifesto externo | Baixo | Isolado |
| `MANIFESTO_RAFAELIA.md` | A | RMR/EXTERNAL | docs | Manifesto externo | Baixo | Isolado |
| `README.md` | M | CORE | docs | README upstream | Médio | Divergência do upstream |
| `RELATORIO.md` | A | RMR/EXTERNAL | docs | Relatório externo | Baixo | Isolado |
| `RELATORIO_AUDITORIA.md` | A | RMR/EXTERNAL | docs | Relatório externo | Baixo | Isolado |
| `audit/diff_bindings.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_c.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_name_status.txt` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_python.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_rmr.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_scripts.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_src.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_stat.txt` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `audit/diff_tools.patch` | A | RMR/EXTERNAL | docs | Patch de auditoria | Baixo | Isolado |
| `b3sum/Cargo.lock` | M | CORE | build | Dependências | Médio | Pode afetar build |
| `b3sum/Cargo.toml` | M | CORE | build | Config do CLI | Médio | Altera build |
| `b3sum/src/main.rs` | M | CORE | runtime | CLI `b3sum` | Médio | Pode afetar comportamento |
| `c/CMakeLists.txt` | M | CORE | build | CMake/flags | Médio | Build C |
| `c/blake3.h` | M | CORE | API | Header público | Alto | Pode afetar API C |
| `c/blake3_c_rust_bindings/Cargo.toml` | M | CORE | build | Bindings Rust/C | Médio | Build bindings |
| `c/blake3_c_rust_bindings/src/lib.rs` | M | CORE | runtime | Bindings Rust/C | Médio | Runtime bindings |
| `c/blake3_dispatch.c` | M | CORE | runtime | Dispatch SIMD | Alto | Núcleo SIMD |
| `c/blake3_impl.h` | M | CORE | runtime | Macros SIMD | Alto | Núcleo SIMD |
| `c/blake3_neon.c` | M | CORE | runtime | SIMD NEON | Alto | Núcleo ARM |
| `c/blake3_neon_aarch64_unix.S` | A | CORE | build/runtime | ASM NEON marker | Médio | Novo ASM |
| `c/blake3_neon_armv7_unix.S` | A | CORE | build/runtime | ASM NEON marker | Médio | Novo ASM |
| `c/test.py` | M | CORE | tests | Teste C | Baixo | Infra de teste |
| `reference_impl/Cargo.toml` | M | CORE | build | Ref impl | Médio | Build crate |
| `rmr/ARCHITECTURE.md` | A | RMR/EXTERNAL | docs | Doc externo | Baixo | Isolado |
| `rmr/LICENSE_RMR` | A | RMR/EXTERNAL | docs | Licença RMR | Baixo | Isolado |
| `rmr/PRIVACY.md` | A | RMR/EXTERNAL | docs | Doc externo | Baixo | Isolado |
| `rmr/PROVENIENCE.md` | A | RMR/EXTERNAL | docs | Proveniência | Baixo | Isolado |
| `rmr/REVIEW.md` | A | RMR/EXTERNAL | docs | Doc externo | Baixo | Isolado |
| `rmr/base.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/base.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/bench.c` | A | RMR/EXTERNAL | runtime | Benchmark externo | Baixo | Isolado |
| `rmr/bench.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/bench_termux.sh` | A | RMR/EXTERNAL | scripts | Script externo | Baixo | Isolado |
| `rmr/benchdiff.c` | A | RMR/EXTERNAL | runtime | Benchmark externo | Baixo | Isolado |
| `rmr/benchdiff.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/benchmark_framework/README.md` | A | RMR/EXTERNAL | docs | Blueprint benchmark | Baixo | Isolado |
| `rmr/benchmark_framework/cli/.gitkeep` | A | RMR/EXTERNAL | build | Placeholder | Baixo | Estrutura |
| `rmr/benchmark_framework/configs/.gitkeep` | A | RMR/EXTERNAL | build | Placeholder | Baixo | Estrutura |
| `rmr/benchmark_framework/core/.gitkeep` | A | RMR/EXTERNAL | build | Placeholder | Baixo | Estrutura |
| `rmr/benchmark_framework/metrics/.gitkeep` | A | RMR/EXTERNAL | build | Placeholder | Baixo | Estrutura |
| `rmr/benchmark_framework/output/.gitkeep` | A | RMR/EXTERNAL | build | Placeholder | Baixo | Estrutura |
| `rmr/build_termux.sh` | A | RMR/EXTERNAL | scripts | Script externo | Baixo | Isolado |
| `rmr/cli.c` | A | RMR/EXTERNAL | runtime | CLI externo | Baixo | Isolado |
| `rmr/cli.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/fix_geom_N_all_scopes.py` | A | RMR/EXTERNAL | tools | Script externo | Baixo | Isolado |
| `rmr/fix_geom_N_scope.py` | A | RMR/EXTERNAL | tools | Script externo | Baixo | Isolado |
| `rmr/fix_pai_warnings.py` | A | RMR/EXTERNAL | tools | Script externo | Baixo | Isolado |
| `rmr/geom.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/geom.c.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/geom.c.pre_fix.1768878317.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/geom.c.pre_fix.1768917982.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/geom.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/geom.pre_fix.1768918156.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/geom.pre_fix.1768918259.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/hash_sha256.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/hash_sha256.c.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/hash_sha256.c.pre_warnfix.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/hash_sha256.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/include/rmr_arch.h` | A | RMR/EXTERNAL | runtime | Headers externos | Baixo | Isolado |
| `rmr/include/rmr_dispatch.h` | A | RMR/EXTERNAL | runtime | Headers externos | Baixo | Isolado |
| `rmr/include/rmr_lowlevel.h` | A | RMR/EXTERNAL | runtime | Headers externos | Baixo | Isolado |
| `rmr/main.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/main.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/pai.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_base.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_bench.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_benchdiff.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_geom.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_hash.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_scan.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_sign.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/pai_toroid.h` | A | RMR/EXTERNAL | runtime | Header externo | Baixo | Isolado |
| `rmr/rust/rmr.rs` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/scan.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/scan.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/sign.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/sign.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/toroid.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/toroid.c.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/toroid.c.pre_warnfix.bak` | A | RMR/EXTERNAL | build | Backup | Baixo | Backup externo |
| `rmr/toroid.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `rmr/util.c` | A | RMR/EXTERNAL | runtime | Código externo | Baixo | Isolado |
| `rmr/util.o` | A | RMR/EXTERNAL | build | Artefato | Baixo | Build externo |
| `src/ffi_avx2.rs` | M | CORE | runtime | SIMD AVX2 | Alto | Núcleo SIMD |
| `src/ffi_avx512.rs` | M | CORE | runtime | SIMD AVX512 | Alto | Núcleo SIMD |
| `src/ffi_neon.rs` | M | CORE | runtime | SIMD NEON | Alto | Núcleo ARM |
| `src/ffi_sse2.rs` | M | CORE | runtime | SIMD SSE2 | Alto | Núcleo SIMD |
| `src/ffi_sse41.rs` | M | CORE | runtime | SIMD SSE4.1 | Alto | Núcleo SIMD |
| `src/guts.rs` | M | CORE | runtime | Núcleo Rust | Alto | Núcleo |
| `src/hazmat.rs` | M | CORE | runtime | Núcleo Rust | Alto | Núcleo |
| `src/join.rs` | M | CORE | runtime | Núcleo Rust | Alto | Núcleo |
| `src/lib.rs` | M | CORE | runtime | API Rust | Alto | Núcleo |
| `src/platform.rs` | M | CORE | runtime | Dispatch Rust | Alto | Núcleo |
| `src/portable.rs` | M | CORE | runtime | Núcleo Rust | Alto | Núcleo |
| `src/rust_avx2.rs` | M | CORE | runtime | SIMD AVX2 | Alto | Núcleo SIMD |
| `src/rust_sse2.rs` | M | CORE | runtime | SIMD SSE2 | Alto | Núcleo SIMD |
| `src/rust_sse41.rs` | M | CORE | runtime | SIMD SSE4.1 | Alto | Núcleo SIMD |
| `src/traits.rs` | M | CORE | runtime | Núcleo Rust | Alto | Núcleo |
| `src/wasm32_simd.rs` | M | CORE | runtime | SIMD WASM | Alto | Núcleo SIMD |
| `test_vectors/Cargo.toml` | M | CORE | build | Test vectors | Baixo | Build/tests |
| `tools/benchmark_example.sh` | A | CORE | tools | Script upstream | Baixo | Ferramenta |
| `tools/compiler_version/Cargo.toml` | M | CORE | build | Tooling | Baixo | Build/tools |
| `tools/instruction_set_support/Cargo.toml` | M | CORE | build | Tooling | Baixo | Build/tools |
| `tools/release.md` | M | CORE | docs | Doc upstream | Baixo | Docs |

### 1.2 Diff estatístico (stat)

> **Fonte:** `git diff --stat upstream/master` (114 arquivos, 12366 insertions, 268 deletions).

---

## 2) Auditoria ARM64/NEON (Passo 2)

**Meta:** identificar configuração real de ARM64/NEON e caminho de dispatch.

### 2.1 Evidências (build/flags)

**Rust / build.rs**
- `build.rs` define `is_aarch64()` e ativa `blake3_neon` quando o target é ARM64
  e little-endian (com exceções para `no_neon`/`pure`).
- `build.rs` adiciona `-mfpu=neon-vfpv4` em ARMv7 quando compila `blake3_neon.c`.

**Cargo features**
- `neon` e `no_neon` são features explícitas no `Cargo.toml`.

**CMake/Makefile**
- `c/CMakeLists.txt` define `BLAKE3_CFLAGS_NEON` e seleciona SIMD `neon-intrinsics`.
- `c/Makefile.testing` suporta `BLAKE3_USE_NEON` e flags específicas.

### 2.2 Evidências (paths NEON/dispatch)

- `c/blake3_impl.h` define `BLAKE3_USE_NEON` para AArch64 (little-endian).
- `c/blake3_dispatch.c` contém blocos `#if BLAKE3_USE_NEON == 1`.
- `src/platform.rs` **não faz detecção dinâmica de NEON**, apenas presume
  suporte quando `blake3_neon` está habilitado.
- Novos arquivos ASM marcadores:
  - `c/blake3_neon_aarch64_unix.S`
  - `c/blake3_neon_armv7_unix.S`

### 2.3 Tabela de auditoria

| build_target | flags_found | neon_path | dispatch_mode | redundancy | confidence |
| --- | --- | --- | --- | --- | --- |
| aarch64 (Rust) | `blake3_neon` via `build.rs` | `src/ffi_neon.rs` + `c/blake3_neon.c` | **Sem detecção dinâmica** | NÃO EVIDENCIADO | Alta |
| armv7 (Rust) | `-mfpu=neon-vfpv4` quando `neon` | `c/blake3_neon.c` | **Sem detecção dinâmica** | NÃO EVIDENCIADO | Alta |
| aarch64 (CMake) | `BLAKE3_CFLAGS_NEON`, `BLAKE3_USE_NEON=1` | `c/blake3_neon.c` | compile-time | NÃO EVIDENCIADO | Alta |
| armv7 (CMake/Makefile) | `-mfpu=neon` / `BLAKE3_USE_NEON` | `c/blake3_neon.c` | compile-time | NÃO EVIDENCIADO | Alta |

> **Nota:** Não há evidência de detecção redundante de NEON (x2). As rotas
> ARM/NEON são habilitadas por compile-time/features, não por runtime.

---

## 3) Metodologia de Benchmarks (Passo 3)

> **Sem execução nesta auditoria.** Somente especificação.

### 3.1 Perfil RAM-only (sem IO)
- Gerar buffer grande em RAM (1–4 GiB).
- Hash contínuo sem logs extras.
- Sem hex output.
- 5 runs → **mediana**.
- Métricas: tempo total + throughput (GiB/s).

### 3.2 Perfil IO+hash (arquivo)
- Ler arquivo grande e hashear.
- Sem logs extras.
- 5 runs → **mediana**.
- Métricas: tempo total + throughput (GiB/s).

### 3.3 Perfil Pipeline RMR
- Pipeline completo (scan/benchdiff/etc.).
- Medir tempo total e por etapa.
- 5 runs → **mediana**.
- Métricas: tempo total + throughput (GiB/s).

### 3.4 Regras gerais
- Registrar: threads, temperatura, seed, tamanho, flags.
- Evitar contaminação por IO/log.
- Reportar **NÃO EVIDENCIADO** quando não medido.

---

## 4) Blueprint do Framework “Industrial” em `rmr/` (Passo 4)

### 4.1 Estrutura proposta (isolada do core)

```
rmr/benchmark_framework/
  README.md
  cli/
  core/
  metrics/
  output/
  configs/
```

### 4.2 Requisitos mínimos

- Reprodutibilidade (seed, configs versionadas).
- Perfis: **ram / io / pipeline**.
- Outputs: **JSON + CSV + Markdown**.
- CLI amigável (ex.: `rmr-bench --profile ram --size 4GiB --runs 5`).
- **BBS/TUI obrigatório**: interface textual interativa.

### 4.3 Métricas iniciais

- Throughput
- Latência
- Variância
- Overhead
- Consistência

---

## 5) Documentação final (Passo 5)

- `RELATORIO.md` (este arquivo).
- `DOCUMENTACAO.md` atualizado com mapa e instruções.
- `rmr/PROVENIENCE.md` atualizado com proveniência do framework.
- `rmr/benchmark_framework/README.md` (opcional, **incluído**).

---

## 6) Conclusões preliminares (sem benchmark)

- Há **múltiplas modificações no core** vs upstream, incluindo SIMD/dispatch.
- ARM64/NEON é ativado por **features/compile-time**, sem detecção dinâmica.
- Performance **NÃO EVIDENCIADA** nesta auditoria (benchmarks não executados).
