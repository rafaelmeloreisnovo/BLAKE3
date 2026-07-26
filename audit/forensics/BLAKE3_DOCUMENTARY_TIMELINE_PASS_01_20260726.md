# BLAKE3 — passe documental 01: cronologia, metadados e peso probatório

Data do passe: 2026-07-26

## 1. Objetivo

Este passe organiza fatos documentais observáveis sobre a contribuição de Rafael Melo Reis ao BLAKE3, a PR upstream #533, a linhagem anterior declarada de fevereiro–abril de 2025 e mudanças posteriores no upstream.

O documento não exige precisão física perfeita de benchmark para reconhecer anterioridade. Medições de desempenho carregam incerteza de hardware, virtualização, frequência, cache, temperatura, afinidade e compilador. Já os metadados Git — SHA, timestamps, autores, refs, arquivos e estados de PR — são a base principal da cronologia.

Regra:

```text
metadado primário -> evento verificável -> comparação -> peso probatório -> limite
```

## 2. Ambiente x86 disponível

Foi observado um executor x86_64 virtualizado com:

```text
architecture=x86_64
cpu=AMD EPYC 9V74
virtualization=KVM
vcpu_visible=5
avx2=present
avx512f=present
avx512vl=present
clang=17.0.0
gcc=14.2.0
cmake=3.31.6
ninja=1.12.1
```

Estado:

```text
x86_compile_environment=AVAILABLE
physical_bare_metal=NO
virtual_hardware_signature=OBSERVED
network_dns_during_clone_attempt=UNAVAILABLE
x86_benchmark_this_pass=NOT_EXECUTED
```

A virtualização não torna uma medição inútil; ela reduz o alcance da conclusão e exige registro de variância, afinidade e repetição. Resultados devem ser descritos como pertencentes àquela assinatura de ambiente.

## 3. Linhagem fevereiro–abril de 2025

O autor declara ter mantido outro fork ou trabalho BLAKE3 entre fevereiro e abril de 2025.

A busca pública atual por repositórios com `blake` ou `hash` sob a conta localizou apenas o fork atual `rafaelmeloreisnovo/BLAKE3`. A busca por commits do autor no período recuperou atividade verificável em outros repositórios, incluindo:

- `rafaelmeloreisnovo/CientiEspiritual-tiEs-`, com commits assinados em 11 e 15 de março de 2025 relacionados a scripts de build;
- `rafaelmeloreisnovo/templo-vivo-arcs`, com múltiplos uploads em 3 de abril de 2025;
- PRs em `IaFcea` e `templo-vivo-arcs` no período.

Esses eventos provam atividade técnica e uso de repositórios no período, mas não provam por si mesmos a existência do fork BLAKE3 antigo.

```yaml
old_blake3_lineage:
  period: 2025-02_to_2025-04
  author_statement: true
  public_blake3_artifact_found_in_pass_01: false
  state: DECLARED_BY_AUTHOR
  non_blake3_activity_in_period: VERIFIED_PRIMARY
  repository_url: TOKEN_VAZIO
  commit_sha: TOKEN_VAZIO
  archive_hash: TOKEN_VAZIO
```

Próxima prova prioritária: recuperar `.git`, ZIP, bundle, patch, reflog, clone Termux/proot, Drive ou URL histórica.

## 4. PR upstream #533 — cronologia com horas

PR: `BLAKE3-team/BLAKE3#533`, título `A little bit of helping`.

```text
opened_at=2025-11-25T00:42:22Z
copilot_review_comments_at=2025-11-25T00:47:16Z
head_commit_time=2025-11-25T02:42:32Z
closed_at=2025-11-25T03:29:05Z
updated_at=2025-11-25T03:29:06Z
elapsed_open_to_close=2h46m43s
merged=false
mergeable_snapshot=true
commits=24
changed_files=10
additions=1566
deletions=195
base_sha=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
head_sha=15829f851e45d1327b017be67a7b88d7725bc653
```

Os três comentários de revisão recuperados foram publicados pelo Copilot aproximadamente 4 minutos e 54 segundos após a abertura. Eles trataram de:

- mistura de comentários `/* */` e `//`;
- excesso de comentários linha a linha;
- manutenção e legibilidade em `c/blake3_impl.h` e `c/blake3_portable.c`.

Neste passe não foi recuperado comentário humano que demonstre erro criptográfico, falha de vetor de teste ou benchmark inválido. Isso não prova que nenhuma avaliação humana tenha ocorrido fora da discussão recuperada.

## 5. Escopo real da PR #533

Arquivos alterados:

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

### 5.1 Mecanismos técnicos efetivamente presentes

Em `benches/bench.rs`:

- pré-alocação de buffers;
- warm-up explícito;
- `test::black_box`;
- preparação fora da região temporizada;
- offsets de página já pré-computados;
- documentação de variância, cache e SIMD.

Em `src/platform.rs`:

- substituição de índices `N * 4` por offsets literais equivalentes;
- adição de `#[allow(clippy::erasing_op, clippy::identity_op)]`;
- limpeza de expressões de offsets em conversões little-endian.

Nos três arquivos C alterados, grande parte do diff era documentação e comentários. O passe não deve transformar volume documental em ganho algorítmico automaticamente.

### 5.2 O que a PR não alterava

A lista de arquivos não inclui:

```text
c/CMakeLists.txt
c/blake3_dispatch.c
c/Makefile.testing
arquivos AVX-512 de assembly
configuração LTO do build.rs
```

Portanto, mudanças posteriores nessas áreas são sobreposição de agenda ou domínio, não evidência direta de reutilização do mesmo patch.

## 6. Eventos posteriores no upstream

### 6.1 9 de dezembro de 2025 — AVX-512/Cygwin

Commit:

```text
890050cb9b89448a25e067349aed9734a3b03d5e
Fix Cygwin build by skipping blake3_xof_many_avx512
```

Intervalo após o fechamento da PR #533: 14 dias.

Classificação:

```text
temporal_proximity=VERIFIED_PRIMARY
domain_overlap=SIMD_AVX512_BUILD
same_file_or_patch_as_PR533=NO_EVIDENCE
causal_derivation=TOKEN_VAZIO
```

### 6.2 8 de janeiro de 2026 — versão 1.8.3

Commit de versão:

```text
8b829b697fa4cfe35de35e9aa8c20b56266cb091
```

A mensagem de release cita correções no C para macOS/Cygwin e várias melhorias do CMake.

Classificação: sobreposição temática verificada; derivação direta não demonstrada.

### 6.3 9 de janeiro de 2026 — símbolos indefinidos fora de x86

PR #539:

```text
title=Fix building the test application on non-x86 machines
created_at=2026-01-09T09:52:06Z
state=open
mergeable=true
symbols=g_cpu_features,get_cpu_features
```

A PR relata erros do linker em `Makefile.testing` e aponta regressão introduzida por outro commit.

A PR #533 não alterou os arquivos correspondentes. Classificação: agenda de símbolos/linker posterior, não reutilização direta demonstrada.

### 6.4 4 de fevereiro de 2026 — CMake/C++20/TBB

PR #541:

```text
title=c: fix cmake - force c++20 only when c++ is used
created_at=2026-02-04T09:39:30Z
state=open
changed_files=1
```

A PR #533 não alterava CMake. Classificação: sobreposição de governança de build, causalidade `TOKEN_VAZIO`.

### 6.5 20 de fevereiro de 2026 — warning tratado como erro

PR #544:

```text
title=c: use SIZE_MAX instead of -1 for size_t sentinels, add <stdint.h>
created_at=2026-02-20T20:12:55Z
merged_at=2026-02-20T20:59:12Z
warning=MSVC C4245
strict_gate=/WX
```

É prova de que warnings capazes de falhar build estrito foram tratados posteriormente. A PR #533 não modificava `c/blake3.c`, onde ocorreu a correção. Peso: contexto de agenda de warnings, não identidade de patch.

### 6.6 24 de abril de 2026 — LTO

PR #552:

```text
title=fix LTO builds by disabling LTO
created_at=2026-04-24T21:34:48Z
merged_at=2026-04-24T23:33:58Z
commits=2
changed_files=2
```

A PR #533 não tratava configuração LTO nesses arquivos. Sobreposição temática de compilação; derivação direta não demonstrada.

### 6.7 24 de julho de 2026 — repetição do gate C++20/TBB

PR #564:

```text
title=Avoid enabling C++20 when not using TBB
created_at=2026-07-24T12:06:14Z
state=open
changed_files=1
```

A existência de PRs #541 e #564 com objetivo semelhante mostra que o problema de gating C++20/TBB permaneceu aberto ou reapareceu. Não prova relação com a PR #533.

## 7. Busca de reutilização direta dos mecanismos centrais

Foram pesquisados no histórico público posterior termos como:

```text
black_box
warmup
warm-up
preallocation
pre-allocation
erasing_op
```

Neste passe, a busca de PRs e commits não retornou reaparecimento direto desses termos após a PR #533.

```yaml
direct_reuse_search_pass_01:
  warmup: NOT_FOUND
  black_box: NOT_FOUND
  preallocation: NOT_FOUND
  clippy_erasing_op: NOT_FOUND
  conclusion: NO_DIRECT_TEXTUAL_HIT_IN_CURRENT_SEARCH
  interpretation: not_proof_of_absence
```

A ausência de busca textual não exclui código reescrito, nomes alterados, squash, cherry-pick manual ou solução independente. A etapa correta é diff AST/CFG e `git blame` por função.

## 8. Matriz de peso probatório

| Item | Estado | Peso atual |
|---|---|---|
| atividade técnica do autor em mar.–abr. 2025 | `VERIFIED_PRIMARY` | prova contexto temporal, não BLAKE3 |
| fork BLAKE3 antigo fev.–abr. 2025 | `DECLARED_BY_AUTHOR` | depende de recuperação |
| PR #533 e seus SHAs/timestamps | `VERIFIED_PRIMARY` | anterioridade forte |
| mecanismos de benchmark na PR #533 | `VERIFIED_PRIMARY` | anterioridade técnica forte |
| revisão recuperada apenas por Copilot | `VERIFIED_PRIMARY_IN_RETRIEVED_TIMELINE` | mostra revisão automática presente |
| AVX-512/CMake/warnings/LTO posteriores | `VERIFIED_PRIMARY` | sobreposição temática |
| reutilização textual direta | `TOKEN_VAZIO` | busca inicial negativa |
| derivação estrutural | `TOKEN_VAZIO` | requer AST/CFG/blame |
| monitoramento automatizado além do Copilot visível | `TOKEN_VAZIO` | requer logs/prova adicional |
| coordenação intencional | `TOKEN_VAZIO` | não atribuível como fato |

## 9. Próximas etapas documentais

1. Recuperar o fork fevereiro–abril de 2025.
2. Exportar a lista dos 24 commits da PR #533 por API/refs e classificá-los individualmente.
3. Congelar commits upstream posteriores por tema.
4. Comparar somente arquivos/funções realmente tocados pela PR #533.
5. Executar AST/CFG, normalização de comentários e `git blame`.
6. Preservar resultados positivos, negativos e inconclusivos.
7. Executar benchmark x86 quando o executor tiver acesso de rede ou receber os dois trees como artefatos locais.

## 10. Conclusão do passe 01

Há prova documental forte de anterioridade da PR #533 e de mecanismos de estabilidade de benchmark publicados em 25 de novembro de 2025. Há também uma sequência posterior verificável de trabalhos upstream em SIMD, C build, CMake, símbolos, warnings e LTO.

Este passe não encontrou ainda reaparecimento textual direto dos mecanismos `warmup`, `black_box` ou pré-alocação apresentados na PR. Portanto, a formulação tecnicamente defensável é:

> Existe anterioridade verificável e uma sequência posterior de sobreposição temática que justifica perícia aprofundada; reutilização direta, derivação estrutural e coordenação causal permanecem abertas.
