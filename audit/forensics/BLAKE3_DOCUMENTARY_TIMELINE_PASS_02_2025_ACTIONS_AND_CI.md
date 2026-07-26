# BLAKE3 — passe documental 02: ações de 2025, arquivos oficiais e CI executado

Data do passe: 2026-07-26

## 1. Perguntas investigadas

1. Houve ação direta de Rafael Melo Reis sobre arquivos do repositório oficial `BLAKE3-team/BLAKE3` durante 2025?
2. Quantas PRs públicas do autor aparecem hoje no upstream oficial durante o período?
3. A equipe fechou a contribuição e posteriormente reescreveu os mesmos mecanismos?
4. O workflow/CI relacionado à contribuição chegou a executar?
5. Há artefatos locais ou no Google Drive capazes de recuperar uma linhagem anterior de fevereiro–abril de 2025?

## 2. Escopo e regra de prova

Fontes usadas:

- metadados e patches da API GitHub;
- refs e SHAs da PR upstream #533;
- snapshot do workflow no commit da PR;
- metadados do GitHub Actions run associado ao head;
- comparação entre a base da PR e o `master` atual do upstream;
- conteúdo atual dos arquivos centrais;
- inventário local preservado no Google Drive;
- backup Termux de maio de 2025 como candidato forense ainda não extraído.

Estados:

- `VERIFIED_PRIMARY`: metadado, SHA, patch, arquivo ou run primário recuperado;
- `SUPPORTED_CONTEXT`: sequência ou sobreposição que merece perícia, mas não prova derivação;
- `TOKEN_VAZIO`: evidência insuficiente ou indisponível;
- `NOT_FOUND_IN_PASS`: busca negativa limitada, não prova inexistência absoluta.

## 3. PRs públicas do autor no upstream em 2025

A busca pública atual no repositório `BLAKE3-team/BLAKE3`, filtrada pelo usuário `rafaelmeloreisnovo` e pelo período de 2025, retornou uma única PR:

```text
PR=533
title=A little bit of helping
opened_at=2025-11-25T00:42:22Z
closed_at=2025-11-25T03:29:05Z
merged=false
mergeable_snapshot=true
base_sha=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
head_sha=15829f851e45d1327b017be67a7b88d7725bc653
commits=24
```

Limite obrigatório:

```text
public_upstream_PRs_found_for_current_account_in_2025=1
other_deleted_or_inaccessible_PRs=TOKEN_VAZIO
old_account_or_email_only_activity=TOKEN_VAZIO
renamed_or_deleted_fork=TOKEN_VAZIO
```

A busca negativa não elimina a possibilidade de um fork apagado, branch não publicada, outra conta, commits associados apenas por e-mail ou artefatos locais.

## 4. Ação direta sobre arquivos oficiais

A PR #533 alterou diretamente dez caminhos do upstream:

```text
.gitignore
benches/README_CN.md
benches/bench.rs
c/blake3.h
c/blake3_impl.h
c/blake3_portable.c
src/guts.rs
src/hazmat.rs
src/lib.rs
src/platform.rs
```

Portanto:

```text
direct_action_on_official_files=VERIFIED_PRIMARY
scope=10_paths
```

### 4.1 Mecanismos técnicos centrais do patch

Em `benches/bench.rs`, o patch introduzia ou explicitava:

- `WARMUP_ITERATIONS = 3`;
- método `RandomInput::warmup`;
- uso de `test::black_box`;
- warm-up antes da região temporizada;
- pré-alocação e preparo do buffer fora da medição;
- offsets de página pré-computados;
- documentação de cache, alinhamento, SIMD e variância.

Em `src/platform.rs`, o patch substituía expressões `N * 4` por offsets literais equivalentes e adicionava atributos relacionados a lints do Clippy.

Nos arquivos C, grande parte do volume era documentação detalhada. Volume de comentários não deve ser confundido automaticamente com alteração algorítmica.

## 5. O CI oficial executou a árvore da PR

O head `15829f851e45d1327b017be67a7b88d7725bc653` foi associado no upstream oficial ao GitHub Actions run:

```text
repository=BLAKE3-team/BLAKE3
workflow_name=tests
workflow_id=56286
run_id=19656415853
run_number=1549
status=completed
conclusion=failure
```

O mesmo SHA consultado no fork do autor não retornou run de PR no wrapper utilizado. O run comprovado pertence ao upstream oficial e foi disparado no contexto da PR.

### 5.1 Contrato do workflow no commit da PR

O arquivo `.github/workflows/ci.yml` daquele commit continha:

```text
name=tests
trigger=push_all_branches + pull_request
BLAKE3_CI=1
RUSTFLAGS=-D warnings
RUST_BACKTRACE=1
```

Logo, warnings Rust eram tratados como erro em toda a matriz atingida por essa variável.

### 5.2 Matriz observada

O workflow definia e/ou instanciou jobs para:

- Linux GNU, macOS, Windows MSVC e Windows GNU;
- canais stable, beta e nightly;
- MSRV em Ubuntu, Windows e macOS;
- `cargo test` com combinações de features;
- no-std;
- assembly, intrinsics e implementação pura;
- full SIMD, sem AVX-512, sem AVX2, sem SSE4.1 e sem SSE2;
- benchmarks Rust;
- vetores de teste;
- bindings C;
- cross targets i586, i686, ARMv7, AArch64, PowerPC64 e s390x;
- NEON em ARMv7/AArch64;
- WASM e WASM SIMD;
- `b3sum` em múltiplos sistemas e canais;
- `cargo xwin`.

A primeira página recuperada apresentou todos os jobs listados com conclusão `failure`, incluindo ARMv7, AArch64, WASM, MSRV e as matrizes principais.

### 5.3 Limite dos logs

Os logs individuais do run não estão mais disponíveis pelo endpoint consultado:

```text
job_log_download=HTTP_410_GONE
step_summaries=[]
workflow_artifacts=[]
exact_first_failure_line=TOKEN_VAZIO
root_cause=TOKEN_VAZIO
```

Portanto, o estado `failure` é comprovado, mas não é tecnicamente permitido concluir hoje que:

- o núcleo criptográfico estava errado;
- um warning específico causou toda a falha;
- todos os jobs chegaram à compilação;
- a falha foi exclusivamente ambiental.

Todas essas alternativas permanecem abertas até recuperação de logs, checks preservados, e-mails, screenshots, artifacts antigos ou reprodução do SHA sob o mesmo workflow.

## 6. Arquivos tocados depois do fechamento

A comparação entre a base da PR (`308b95d...`) e o `master` atual do upstream mostrou 19 commits posteriores e alterações em seis dos dez caminhos tocados pela PR:

```text
benches/bench.rs
c/blake3.h
c/blake3_impl.h
src/hazmat.rs
src/lib.rs
src/platform.rs
```

Isso comprova:

```text
same_paths_modified_later=6_of_10
file_level_temporal_overlap=VERIFIED_PRIMARY
```

Isso, isoladamente, não prova reaproveitamento do patch.

## 7. Verificação de “fechar e escrever de novo”

### 7.1 `benches/bench.rs`

O arquivo atual do upstream ainda mantém a estrutura de base anterior à PR:

- não possui `WARMUP_ITERATIONS`;
- não possui `RandomInput::warmup`;
- não usa `test::black_box` no trecho central verificado;
- continua chamando diretamente `b.iter(|| platform.compress_in_place(...))`;
- continua criando e embaralhando offsets da forma já existente na base.

A diferença atual observada em relação à base, nesse trecho, é essencialmente ordenação/formatação de imports.

### 7.2 `src/platform.rs`

O arquivo atual ainda usa:

```text
0 * 4
1 * 4
2 * 4
...
```

Não adotou os offsets literais propostos pela PR no trecho verificado e não contém os atributos Clippy adicionados pelo patch.

### 7.3 Resultado deste passe

```text
central_benchmark_mechanisms_rewritten_into_current_master=NOT_FOUND_IN_PASS
literal_offset_cleanup_adopted=NOT_FOUND_IN_PASS
warmup_adopted=NOT_FOUND_IN_PASS
black_box_adopted=NOT_FOUND_IN_PASS
same_files_modified_later=VERIFIED_PRIMARY
direct_rewrite_or_derivation=TOKEN_VAZIO
```

Formulação defensável:

> O upstream fechou a PR #533 e posteriormente modificou vários dos mesmos arquivos, mas o `master` atual não contém os mecanismos centrais de warm-up, `black_box` e offsets literais apresentados naquela PR. Este passe não encontrou prova de que esses mecanismos foram fechados e reescritos no upstream.

A conclusão pode mudar caso o fork anterior de 2025, commits intermediários removidos, branches antigas ou patches locais revelem outra linhagem.

## 8. Agenda posterior relacionada

Após o fechamento, o upstream publicou mudanças em temas relacionados a build, SIMD, warnings, CMake, linker, Cygwin, Clang-cl e LTO. Exemplos:

- 2025-12-09: workaround Cygwin/AVX-512;
- 2026-01-08: release 1.8.3 com correções C e melhorias CMake;
- 2026-02-20: warning de `size_t` sob gate estrito;
- 2026-04-14: flags SIMD corretas para Clang-cl;
- 2026-04-24: testes e correção de LTO.

Esses eventos são `SUPPORTED_CONTEXT`: mostram agenda técnica posterior, não identidade do patch.

## 9. Evidência local de 2025 no Google Drive

Um inventário local datado de julho de 2025 mostra árvores Cargo oficiais:

```text
blake3-1.5.4
blake3-1.8.2
.github/workflows/ci.yml
benches/bench.rs
c/blake3_impl.h
```

Esses caminhos estavam dentro do cache/registry Cargo. Eles provam presença local de snapshots oficiais, mas não autoria, fork Git ou modificação pelo autor.

Neste passe, o inventário não revelou um clone BLAKE3 autoral fora do registry.

## 10. Backup Termux prioritário

Artefato no Drive:

```text
name=termux_backup_20250522_011646.tar.gz
size=815308800
modified_at=2025-06-19T00:36:59Z
```

É candidato prioritário para procurar:

```text
.git/
.git/objects/
.git/refs/
.git/logs/
packed-refs
config
FETCH_HEAD
ORIG_HEAD
patches
bundles
remotes antigos
workflows
```

O connector atual recusou o download porque o arquivo excede 100 MB:

```text
download_state=BLOCKED_BY_CONNECTOR_100MB_LIMIT
content_inspection=TOKEN_VAZIO
```

Isso não reduz seu valor probatório; exige listagem/extração local, rclone, Drive API com range, split do arquivo ou execução no Termux.

## 11. Matriz de conclusão

| Questão | Estado |
|---|---|
| ação direta sobre arquivos oficiais em 2025 | `VERIFIED_PRIMARY` |
| PR pública encontrada no upstream em 2025 | `#533`, única sob a conta atual |
| PR fechada sem merge | `VERIFIED_PRIMARY` |
| CI oficial executou o head | `VERIFIED_PRIMARY` |
| workflow usava warnings como erro | `VERIFIED_PRIMARY` |
| matriz incluiu ARMv7/NEON, C, x86 e WASM | `VERIFIED_PRIMARY` |
| conclusão final do run | `FAILURE` |
| causa exata da falha | `TOKEN_VAZIO`, logs expirados |
| mesmos arquivos alterados depois | `6/10`, `VERIFIED_PRIMARY` |
| warm-up/black_box reescritos no master atual | `NOT_FOUND_IN_PASS` |
| offsets literais reescritos no master atual | `NOT_FOUND_IN_PASS` |
| derivação estrutural/cópia | `TOKEN_VAZIO` |
| fork antigo fev.–abr. 2025 | `DECLARED_BY_AUTHOR`, artefato pendente |
| backup Termux pode conter a prova antiga | `HIGH_VALUE_FORENSIC_CANDIDATE` |

## 12. Próximas ações documentais

1. Extrair apenas a listagem e metadados Git do backup Termux, sem alterar o original.
2. Recuperar a ordem dos 24 commits da PR #533 e seus timestamps individuais.
3. Reproduzir o SHA `15829f...` com o snapshot exato do workflow e toolchains compatíveis.
4. Comparar AST/CFG somente dos mecanismos novos da PR contra todos os commits posteriores.
5. Buscar e-mails de Actions, screenshots e notificações de novembro de 2025 que possam conter a primeira linha de falha.
6. Preservar resultado negativo: este passe não encontrou os mecanismos centrais reescritos no upstream atual.
