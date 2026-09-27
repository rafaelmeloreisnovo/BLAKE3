# BLAKE3 oficial — cadeia de custódia do CI e dos checks

**Schema:** `BLAKE3_OFFICIAL_CI_CHAIN_OF_CUSTODY_V1`  
**Snapshot:** `2026-09-27T20:52:00-03:00`  
**SOURCE autoritativa:** `BLAKE3-team/BLAKE3`  
**FORK de custódia:** `rafaelmeloreisnovo/BLAKE3`  
**Regra:** `SOURCE != FORK != EXECUTION != CHECK != CLAIM`.

## 1. Âncoras oficiais observadas

| Objeto | Valor |
|---|---|
| repositório oficial | `BLAKE3-team/BLAKE3` |
| branch padrão | `master` |
| HEAD oficial observado | `6aab490a26124663329dfd3961b8469f8fdb158b` |
| HEAD date | `2026-09-10T20:47:09Z` |
| HEAD message | `align the two on-stack block buffers to 64 bytes (#582)` |
| `.github/workflows/ci.yml` blob | `f7dd8c053acc2c215032a17482273a2ca00d90e5` |
| `.github/workflows/tag.yml` blob | `61be4ff991c29d481cc0b2017ed3c3200585b6b0` |
| `.github/workflows/build_b3sum.py` blob | `f0e1787c75e8e40320045ccdd3853ce8dea7a52d` |
| `.github/workflows/upload_github_release_asset.py` blob | `76340bee3986b43c78ee0a140b1cb326e9040029` |

No mesmo snapshot, o fork possuía os mesmos blobs do oficial para `ci.yml` e `tag.yml`.

## 2. O que bloqueia push/merge no oficial neste snapshot

A API do GitHub reportou para `BLAKE3-team/BLAKE3:master`:

- `protected=false`;
- branch protection `enabled=false`;
- required status check contexts: vazio;
- rulesets retornados: `[]`.

Isto é um **snapshot factual** da superfície de API observada em 2026-09-27, não uma afirmação permanente. Nesta fotografia não foi encontrado required check/ruleset que torne os Actions uma pré-condição de aceitação de push direto em `master`.

Logo, nesta superfície:

```text
push aceito
   -> commit existe na master
      -> Actions executam
         -> checks podem concluir depois
```

`CHECK_FAIL != PUSH_REJECTED`.

## 3. Estrutura atual do CI oficial

O `ci.yml` atual possui **505 linhas** e **15 jobs top-level**:

1. `library_tests`
2. `msrv_build`
3. `b3sum_tests`
4. `cross_tests`
5. `wasm_tests`
6. `cargo_xwin_test`
7. `cmake_c_tests`
8. `pkg_config_c_tests`
9. `build_apple_silicon`
10. `build_tinycc`
11. `gcc54`
12. `cmake_current_build`
13. `cmake_3-9_build`
14. `miri_smoketest`
15. `tbb_rust_bindings_tests`

Triggers observados:

```yaml
on:
  push:
    branches:
      - master
  pull_request:
```

Controles relevantes:

```text
fail-fast:false        = 9 ocorrências
needs:                 = 0
concurrency:           = 0
if: always()           = 0
continue-on-error      = 0
paths:                 = 0
vars.*                 = 0
secrets.* em ci.yml    = 0
permissions:           = 0
timeout-minutes:       = 0
```

Portanto, no **oficial**, não existe o padrão RMR de `aggregate/summary + needs.*.result + always()`. A falha tardia no oficial decorre principalmente de checks independentes/matrizes que terminam em tempos diferentes.

## 4. Prova de checks ocultáveis por paginação

No HEAD oficial `6aab490...`:

```text
GET /commits/<sha>/check-runs
total_count = 74
returned    = 30

GET /commits/<sha>/check-runs?per_page=100
returned    = 74
```

No snapshot completo:

```text
74 checks
74 success
0 failure
0 cancelled
```

Janela observada de execução:

```text
primeiro início: 2026-09-10T20:47:15Z
última conclusão: 2026-09-10T20:55:45Z
```

Logo, um cliente que lê apenas a primeira página pode ver **30 verdes e omitir 44 checks**. Para cadeia de custódia, `PASS` só é promovido quando:

```text
pagination_complete == true
returned_total == total_count
all_checks_terminal == true
all_required_scope_checks_success == true
```

## 5. Por que pode parecer “verde e depois vermelho”

O oficial usa `fail-fast:false` para preservar a execução das demais células da matriz.

Dois commits são âncoras diretas dessa intenção:

### 2020-02-06 — matriz principal

Commit:

`ca62c4724d70a523af0ed34c8993643cdbf05269`

Autor Git: `oconnor663`

Mensagem:

`stop skipping all other builds when one CI build fails`

Hunk relevante:

```diff
 strategy:
+  fail-fast: false
   matrix:
```

### 2020-03-24 — cross tests

Commit:

`4feadee6bbbba2fc686d7e6204d8d6f104b3803e`

Autor Git: `oconnor663`

Mensagem:

`disable fail-fast for cross tests too`

Hunk relevante:

```diff
 strategy:
+  fail-fast: false
   matrix:
```

Assim, vários checks podem estar verdes enquanto outros ainda continuam. Um check que conclua mais tarde pode mudar o estado global percebido. Isso é compatível com a política explícita de coleta completa da matriz; não é evidência de ocultação deliberada.

## 6. Marcos de evolução do workflow oficial

| Data UTC | Commit | Autor Git | Fato documental |
|---|---|---|---|
| 2019-12-10 | `98dd9cbbf1bc` | oconnor663 | criação de `ci.yml` |
| 2020-01-10 | `9096249e0963` | oconnor663 | habilitou CI também em pull requests |
| 2020-01-20 | `4021636022cf` | oconnor663 | adicionou testes das variáveis `BLAKE3_NO_*` |
| 2020-02-06 | `ca62c4724d70` | oconnor663 | adicionou `fail-fast:false` à matriz principal |
| 2020-03-24 | `4feadee6bbbbb` | oconnor663 | adicionou `fail-fast:false` aos cross tests |
| 2022-11-20 | `e067e7f49839` | oconnor663 | adicionou toolchain MSRV ao CI |
| 2023-12-28 | `5306464d031f` | striezel | checkout atualizado para v4 |
| 2023-12-30 | `4d32708f511f` | striezel | substituiu action de toolchain não mantida |
| 2025-03-18 | `4011d240cb02` | oconnor663 | adicionou WASM tests |
| 2025-04-03 | `bafe693a8238` | silvanshade | adicionou testes pkg-config |
| 2026-04-24 | `6a45feedc618` | oconnor663 | adicionou cobertura LTO e Linux arm64 |
| 2026-07-27 | `fc3d0e98e840` | nazar-pc | alterou path Visual Studio |
| 2026-08-03 | `9eac279fd71b` | oconnor663 | passou a localizar Visual Studio via `vswhere` |

O histórico integral de **92 commits** que tocaram `.github/workflows/ci.yml` foi preservado separadamente em:

`audit/forensics/BLAKE3_OFFICIAL_CI_HISTORY_2019_2026_20260927.tsv`

Esse ledger guarda timestamp, SHA completo, identidade Git de autor, identidade do committer, primeira linha da mensagem e URL upstream.

## 7. Variáveis/condições que alteram o corpo de checks

A expansão atual é governada principalmente por:

- `matrix.target.name`
- `matrix.target.os`
- `matrix.target.toolchain`
- `matrix.channel`
- `matrix.arch`
- `matrix.simd`
- `matrix.use_tbb`
- `matrix.shared_libs`
- `matrix.stdlib`
- `matrix.toolchain.cc`
- `matrix.toolchain.cxx`
- `github.workspace`

Há condições `if:` para toolchain, arquitetura e sistema operacional, incluindo:

- exclusão de MSVC em determinados testes LTO;
- passos específicos para i586;
- NEON/no-NEON específicos para ARMv7/AArch64;
- TBB diferenciado por Ubuntu/macOS.

Não foram encontrados no `ci.yml`:

```text
${{ vars.* }}
${{ secrets.* }}
needs.*.result
environment:
workflow_run:
merge_group:
continue-on-error:
if: always()
concurrency:
```

No `tag.yml`, por outro lado, existem:

```yaml
GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
GITHUB_TAG: ${{ github.ref }}
```

## 8. Dependências móveis / reprodutibilidade

O `ci.yml` oficial atual contém referências que não são todas imutáveis:

- `actions/checkout@v4`;
- `dtolnay/rust-toolchain@master`;
- `dtolnay/rust-toolchain@stable`;
- `dtolnay/rust-toolchain@nightly`;
- `cargo install cross` sem versão fixa;
- `curl https://wasmtime.dev/install.sh -sSf | bash`;
- `messense/cargo-xwin`;
- imagem Docker `gcc:5.4`.

Há também um pin por SHA completo:

`lukka/get-cmake@5f6e04f5267c8133f1273bf2103583fc72c46b17`.

Classificação deste ponto: `GAP_REPRODUCIBILITY`, não defeito criptográfico.

## 9. Workflow oficial de tags

`tag.yml` possui **44 linhas**, um job top-level e dispara em push de tags.

Ele usa uma matriz de targets e chama:

`python -u .github/workflows/build_b3sum.py <target>`

Depois utiliza:

```yaml
GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
GITHUB_TAG: ${{ github.ref }}
```

para upload dos assets.

O histórico retornado para `tag.yml` contém 4 commits; o ponto de origem funcional é o commit `ba468fbb4f7bb8b149eea60d0eab37858d32f23c` de 2020-04-28, “build b3sum binaries in CI for new tags”.

## 10. Script oficial `build_b3sum.py`

No snapshot atual, o script chama `subprocess.run(["cargo","build",...])` sem `check=True` e ainda emite a forma histórica `::set-output name=bin_path::...`.

Isto é preservado como **superfície de robustez/modernização**. Não é promovido a claim de falso PASS sem uma execução que demonstre esse resultado.

## 11. Separação obrigatória do oficial e do fork

```text
OFICIAL ci.yml/tag.yml
  = SOURCE upstream

RMR workflows
  = extensão do fork

aggregate/summary/always()/needs.*.result/concurrency
  = observados em workflows RMR
  != comportamento do ci.yml oficial
```

Assim, qualquer investigação sobre “commit final falha” precisa primeiro identificar qual workflow gerou o check:

```text
check -> details_url -> actions/run -> workflow path -> blob SHA -> commit SHA
```

Só depois se pode atribuir o comportamento ao oficial ou ao fork.

## 12. Claims gate deste snapshot

```text
OFFICIAL_HEAD=6aab490a26124663329dfd3961b8469f8fdb158b
OFFICIAL_CI_BLOB=f7dd8c053acc2c215032a17482273a2ca00d90e5
OFFICIAL_TAG_BLOB=61be4ff991c29d481cc0b2017ed3c3200585b6b0
OFFICIAL_HEAD_CHECKS_TOTAL=74
OFFICIAL_HEAD_CHECKS_RETURNED_DEFAULT=30
OFFICIAL_HEAD_CHECKS_RETURNED_COMPLETE=74
OFFICIAL_HEAD_ALL_CHECKS_SUCCESS=true
OFFICIAL_MASTER_PROTECTED=false
OFFICIAL_RULESETS_COUNT=0
OFFICIAL_CI_HAS_FINAL_AGGREGATOR=false
OFFICIAL_CI_HAS_NEEDS=false
OFFICIAL_CI_HAS_ALWAYS=false
OFFICIAL_CI_HAS_CONCURRENCY=false
OFFICIAL_CI_HISTORY_PATH_COMMITS=92
INTENTIONAL_HIDING_CLAIM=TOKEN_VAZIO
MOTIVE_CLAIM=TOKEN_VAZIO
LEGAL_RESPONSIBILITY_CLAIM=TOKEN_VAZIO
```

## 13. Limites forenses

- configuração de GitHub é mutável e deve ser resnapshotada;
- identidade Git de autor/committer é metadado documental e não prova intenção ou autoria exclusiva;
- `PASS` significa apenas conclusão bem-sucedida naquele commit/ambiente;
- primeira página de checks não é suficiente quando `total_count > returned`;
- alteração de workflow ao longo do tempo não prova motivo;
- nenhum fato deste documento imputa fraude, sabotagem ou ocultação deliberada.

## R3

`F_ok`: SOURCE oficial fixada por HEAD e blobs; 74/74 checks do HEAD foram observados como success; paginação 30/74 comprovada; 92 commits históricos do `ci.yml` preservados em ledger.  
`F_gap`: dependências móveis reduzem reprodutibilidade; proteção/rulesets podem mudar após o snapshot; identidade e motivação além do Git permanecem fora do gate.  
`F_next`: gerar successor append-only quando HEAD, blobs de workflow, branch protection, rulesets ou topologia dos checks mudarem.
