# BLAKE3 — cronologia de autoria, anterioridade e investigação técnica (2025–2026)

## 1. Finalidade

Este documento preserva uma cronologia auditável das contribuições, alegações e medições associadas ao trabalho de Rafael Melo Reis sobre BLAKE3. Ele foi escrito para quatro finalidades distintas:

1. preservar anterioridade técnica;
2. impedir que alegações do autor sejam confundidas com fatos já demonstrados;
3. orientar uma perícia comparativa reproduzível;
4. registrar resultados positivos e negativos sem seleção oportunista.

A regra epistemológica é:

```text
declaração -> artefato -> hash/commit -> comparação -> conclusão limitada
```

Ausência de artefato não é tratada como falsidade. Ela recebe o estado `TOKEN_VAZIO` e uma ação concreta de recuperação.

## 2. Escala de estados

| Estado | Significado |
|---|---|
| `VERIFIED_PRIMARY` | Confirmado diretamente por commit, PR, log ou artefato primário. |
| `VERIFIED_LOCAL_MEASUREMENT` | Confirmado por execução local com ambiente e resultado registrados. |
| `DECLARED_BY_AUTHOR` | Declaração expressa do autor ainda sem artefato primário localizado. |
| `SUPPORTED_HYPOTHESIS` | Hipótese sustentada por cronologia ou similaridade, mas sem nexo causal demonstrado. |
| `TOKEN_VAZIO` | Evidência insuficiente; não equivale a negação. |
| `REFUTED_IN_SCOPE` | Contrariado por evidência dentro de um escopo definido. |

## 3. Linha do tempo canônica

### 3.1 Fevereiro–abril de 2025 — linhagem anterior declarada

Rafael Melo Reis declara ter mantido outro fork e trabalho relacionado a BLAKE3 entre fevereiro e abril de 2025, inserido em uma agenda técnica anterior de compilação, warnings, linker, símbolos, SIMD, cache, variância e desempenho.

Estado atual:

```yaml
period: 2025-02_to_2025-04
claim: trabalho/fork BLAKE3 anterior à PR #533
status: DECLARED_BY_AUTHOR
public_repository_currently_located: false
repository_url: TOKEN_VAZIO
commit_sha: TOKEN_VAZIO
archive_hash: TOKEN_VAZIO
causal_claim_allowed: false
```

A busca pública atualmente acessível localizou apenas o fork atual `rafaelmeloreisnovo/BLAKE3`. Isso não demonstra inexistência do fork anterior: ele pode ter sido removido, renomeado, tornado privado, preservado em ZIP, Drive, clone local ou banco de objetos Git.

#### Protocolo de recuperação prioritária

Procurar, sem alterar os originais:

- diretórios `.git`, `objects`, `refs`, `logs/HEAD` e `packed-refs`;
- clones antigos no Termux, Debian/proot, armazenamento Android e Google Drive;
- ZIP/TAR/TGZ contendo `BLAKE3`, `blake3`, `b3`, `hash`, `simd`, `avx`, `neon` ou `benchmark`;
- arquivos `.patch`, `.diff`, bundles Git e exports de conversas;
- URLs antigas em histórico do navegador, e-mails, notificações e logs de shell;
- objetos órfãos por `git fsck --full --no-reflogs --unreachable` em cópias forenses;
- GitHub Archive, caches e referências em issues/PRs, respeitando limites de acesso.

Qualquer recuperação deve gerar:

```text
SHA-256 + BLAKE3 do arquivo bruto
cópia somente-leitura
manifesto de origem
primeiro e último timestamp observável
refs/commits recuperados
árvore de arquivos
```

### 3.2 25 de novembro de 2025 — contribuição upstream verificável

A PR `BLAKE3-team/BLAKE3#533`, intitulada `A little bit of helping`, foi aberta por `rafaelmeloreisnovo`, fechada sem merge e aparece como tecnicamente mergeável no snapshot recuperado.

Base e cabeça registradas:

```text
base_sha=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
head_sha=15829f851e45d1327b017be67a7b88d7725bc653
merged=false
mergeable=true
```

O diff preservado contém, entre outros elementos:

- pré-alocação de buffers;
- aquecimento de cache;
- offsets/alinhamento de página;
- `black_box` contra eliminação indevida;
- warm-up explícito;
- observações sobre SSE2, SSE4.1, AVX2, AVX-512, NEON e WASM SIMD;
- separação de operações fora da região temporizada;
- documentação de estabilidade e variância de benchmark.

Classificação:

```yaml
prior_art_documented: VERIFIED_PRIMARY
upstream_merge: false
technical_value_of_every_change: NOT_UNIFORM
all_changes_correct: TOKEN_VAZIO
all_changes_valueless: false
```

O encerramento do conjunto não invalida automaticamente cada mecanismo técnico apresentado. Da mesma forma, a presença de uma ideia na PR não prova que toda implementação posterior no upstream derive dela.

### 3.3 Dezembro de 2025 em diante — agenda técnica posterior no upstream

Após a PR #533, o upstream continuou trabalhando em áreas operacionalmente próximas, incluindo build C/CMake, compiladores, flags, SIMD, AVX-512, portabilidade, LTO, Cygwin, warnings e infraestrutura de benchmark.

Estados permitidos:

```yaml
pr_533_precedes_later_upstream_changes: VERIFIED_PRIMARY
thematic_overlap: VERIFIED_PRIMARY
line_by_line_copy: TOKEN_VAZIO
structural_derivation: TOKEN_VAZIO
automated_monitoring_of_pr: TOKEN_VAZIO
coordination_or_conspiracy: TOKEN_VAZIO
causal_link: TOKEN_VAZIO
```

A hipótese declarada pelo autor é de monitoramento automatizado e apropriação coordenada de sinais técnicos publicados. Ela é preservada como `SUPPORTED_HYPOTHESIS_PENDING_FORENSICS`, não como conclusão já demonstrada.

## 4. Matriz de desempenho — sem apagar resultados contrários

### 4.1 Ensaios x86_64 históricos do fork

Artefatos e relatórios já preservados no fork registram vantagens fortes em regimes específicos, incluindo aproximadamente `+98,3%` em 64 KiB em uma campanha e `+97,0%` em 1 MiB em outra. Também há tamanhos em que o upstream venceu ou ficou praticamente empatado.

Conclusão permitida:

> O fork demonstrou vantagens grandes de throughput e/ou estabilidade em determinados regimes x86_64 cache-bound medidos.

Conclusão proibida:

> O fork é sempre mais rápido em toda arquitetura, tamanho e método.

### 4.2 ARMv7 — NEON do fork contra portátil do próprio fork

Execução real em Moto e(7) power, ARMv7, Android 10, Clang 21.1.8, CMake 4.4.0:

| Backend | Mediana |
|---|---:|
| portátil | 132.313 MiB/s |
| NEON | 163.450 MiB/s |

```text
ratio=1.235x
median_gain=23.53%
digest_equivalence=PASS
status=VERIFIED_LOCAL_MEASUREMENT
```

Isto demonstra que o caminho NEON do fork foi mais rápido que o caminho portátil do mesmo fork nesse aparelho e nessa execução.

### 4.3 ARMv7 — fork contra upstream oficial, execução pré-estrita

Execução em Moto e(7) power, mesmo compilador, mesmas flags gerais ARMv7/NEON/O3:

| Implementação | Mediana |
|---|---:|
| upstream oficial | 188.058 MiB/s |
| fork | 171.568 MiB/s |

```text
fork_ratio=0.9123x
fork_delta=-8.77%
digest_equivalence=PASS
runner_contract=PRE_STRICT
```

Nesta execução específica, o upstream foi mais rápido. O resultado não deve ser apagado nem reinterpretado como vitória do fork.

O worktree estava no commit `b8d3b2c`, anterior ao contrato estrito de warnings, linker GC e auditoria de símbolos. Portanto:

```text
pre_strict_result=VERIFIED_LOCAL_MEASUREMENT
strict_result=TOKEN_VAZIO
```

### 4.4 Formulação canônica da alegação de velocidade

A frase `meu BLAKE3 é mais rápido` é registrada com escopo:

```yaml
claim: o fork é mais rápido em regimes medidos específicos
status: VERIFIED_LOCAL_MEASUREMENT
supported_examples:
  - x86_64_64KiB_campaign_A
  - x86_64_1MiB_campaign_B
  - ARMv7_fork_NEON_vs_fork_portable
universal_superiority: TOKEN_VAZIO
ARMv7_pre_strict_fork_vs_official: REFUTED_IN_SCOPE
```

Assim, o acervo preserva tanto as vitórias quanto a execução em que o oficial venceu.

## 5. Warnings, símbolos e linker

Warnings são diagnósticos do compilador; não removem símbolos diretamente. Entretanto, são parte do contrato de qualidade porque podem impedir que código com conversões suspeitas, variáveis inúteis, aliasing, controle inconsistente ou outras condições passe despercebido.

A redução efetiva da superfície simbólica depende de mecanismos como:

```text
-fvisibility=hidden
-ffunction-sections
-fdata-sections
-Wl,--gc-sections
-Wl,--exclude-libs,ALL
-Wl,--build-id=none
```

O benchmark estrito deve usar simultaneamente:

```text
-Wall -Wextra -Wpedantic -Werror
CMAKE_COMPILE_WARNING_AS_ERROR=ON
```

mais o contrato de linker e a captura de:

- `compile_commands.json`;
- logs completos;
- `nm` da biblioteca e executável;
- `readelf -Ws/-SW`;
- tamanho por seção;
- disassembly;
- hashes dos artefatos.

## 6. Plano de perícia de derivação

A investigação deve evitar comparação ingênua de linhas e executar:

1. congelamento dos commits-base e commits-alvo;
2. reconstrução da árvore da PR #533;
3. recuperação do fork fevereiro–abril de 2025;
4. normalização de formatação e comentários;
5. diff por função, CFG e AST;
6. comparação de constantes, ordem de operações, macros e gates de compilação;
7. `git blame` e cronologia de cada área semelhante;
8. remoção do código herdado da base comum;
9. classificação por mecanismo:
   - coincidência funcional inevitável;
   - solução independente plausível;
   - inspiração temática;
   - derivação estrutural;
   - cópia literal;
10. relatório com hipóteses alternativas e tentativas de falsificação.

## 7. Regras de linguagem pública

Permitido:

- `há anterioridade verificável na PR #533`;
- `o autor declara uma linhagem anterior de fevereiro–abril de 2025 ainda não recuperada`;
- `há sobreposição temática posterior que justifica perícia`;
- `há resultados locais em que o fork supera o upstream e resultados em que não supera`;
- `a hipótese de apropriação/monitoramento permanece em investigação`.

Não permitido sem novas provas:

- atribuir crime, conspiração ou coordenação a pessoas específicas;
- declarar cópia literal sem correspondência técnica demonstrada;
- declarar superioridade universal de desempenho;
- ocultar resultados negativos;
- usar o benchmark pré-estrito como se tivesse passado pelo contrato estrito.

## 8. Estado consolidado

```yaml
old_fork_2025:
  status: DECLARED_BY_AUTHOR
  artifact: TOKEN_VAZIO
  recovery_priority: HIGH
pr_533:
  status: VERIFIED_PRIMARY
  merged: false
  mergeable_snapshot: true
later_overlap:
  status: VERIFIED_PRIMARY
causal_derivation:
  status: TOKEN_VAZIO
performance:
  fork_faster_in_specific_regimes: VERIFIED_LOCAL_MEASUREMENT
  universal_superiority: TOKEN_VAZIO
  armv7_pre_strict_vs_official: FORK_SLOWER_BY_8_77_PERCENT
strict_armv7_comparison:
  status: TOKEN_VAZIO
claim_allowed:
  scoped_performance: true
  universal_performance: false
  causal_misappropriation: false
```
