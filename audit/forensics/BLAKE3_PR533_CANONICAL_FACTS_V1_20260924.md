<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.

This document records public technical evidence. It is not a judicial finding
and does not attribute unlawful intent without specific independent evidence.
-->

# BLAKE3 — Manifesto Forense Canônico de Fatos e Provas V1

**Autor da compilação forense:** Rafael Melo Reis  
**Repositório de custódia:** `rafaelmeloreisnovo/BLAKE3`  
**Fonte primária principal:** `BLAKE3-team/BLAKE3`  
**Snapshot:** 2026-09-24  
**Classe:** `CANONICAL_FORENSIC_FACTS`  
**Estado:** `SOURCE_BOUND / APPEND_ONLY / FAIL_CLOSED / NON_ACCUSATORY`  
**Predecessor:** `BLAKE3_FORENSIC_MANIFESTO_NONNEGOTIABLE_V1_20260924.md`

## 0. Regra de leitura

Este documento registra fatos públicos verificáveis por GitHub: PRs, commits,
SHAs, datas, diffs, comentários e estado atual do código.

A cadeia obrigatória é:

```text
SOURCE -> SHA/PR -> DIFF -> TIMELINE -> COMPARISON -> LIMITED CLAIM
```

e:

```text
SOURCE != ARTEFATO != EXECUCAO != EVIDENCIA != CLAIM
TOKEN_VAZIO != 0 != PROVA
```

Uma correspondência de código pode provar anterioridade e igualdade textual.
Ela não prova, sozinha, que um autor posterior consultou a fonte anterior.
Uma sequência temporal pode provar ordem. Ela não prova, sozinha, intenção.

Este documento não declara fraude, plágio, apropriação ilícita, conspiração,
coordenação indevida ou finalidade anticompetitiva. Tais conclusões exigiriam
prova adicional e, quando aplicável, processo jurídico competente.

---

## 1. Fonte central — PR upstream #533

**PR:** `BLAKE3-team/BLAKE3#533`  
**Título:** `A little bit of helping`  
**Autor GitHub:** `rafaelmeloreisnovo`  
**Head repository:** `rafaelmeloreisnovo/BLAKE3`  
**Base SHA:** `308b95dfa15d5a0aa8cb3c5534ffd90d76122c46`  
**Head SHA:** `15829f851e45d1327b017be67a7b88d7725bc653`  
**Criada:** `2025-11-25T00:42:22Z`  
**Fechada:** `2025-11-25T03:29:05Z`  
**Merged:** `false`  
**Commits:** 24  
**Arquivos alterados:** 10  
**Adições:** 1566  
**Remoções:** 195

Arquivos:

1. `.gitignore`
2. `benches/README_CN.md`
3. `benches/bench.rs`
4. `c/blake3.h`
5. `c/blake3_impl.h`
6. `c/blake3_portable.c`
7. `src/guts.rs`
8. `src/hazmat.rs`
9. `src/lib.rs`
10. `src/platform.rs`

No fechamento, o mantenedor `oconnor663` publicou:

> “Please do not open PRs like this. This is not a good use of our time.”

Este documento registra essa frase apenas como evento de timeline.

### 1.1 Proveniência automatizada anterior à abertura formal

A timeline da #533 registra commits de `copilot-swe-agent[bot]` antes de
`2025-11-25T00:42:22Z`, horário de criação formal da PR, incluindo:

- `f7c01ff3907560b53e4c654e18386938925c38fc` — 00:04:02Z
- `cd94ec5b3e320bfea05c9bfc2bdbd156fde0977a` — 00:14:04Z
- `92ad42000bac5635d6a3f876c8f5576be891a283` — 00:16:46Z
- `358dc1f462288de8291795aee8386a1dd5cc3776` — 00:17:37Z
- `2cd1b6b4c9cc38de45f3bc1a9b3566ed02e8db58` — 00:28:38Z

**Fato permitido:** parte do conteúdo da branch foi produzida antes da abertura
formal do objeto PR.

**Limite:** esses commits pertencem à branch de origem da própria #533. Eles
não constituem evidência de pré-injeção pelo upstream.

---

# 2. Prova canônica E-533-001 — correspondência literal posterior

## 2.1 Alteração na PR #533

Em `src/lib.rs`, dentro da implementação de
`std::io::Seek for OutputReader`, a #533 contém:

```diff
- let max_position = u64::max_value() as i128;
+ let max_position = u64::MAX as i128;
```

Esta alteração está presente no diff público da #533 em 2025-11-25.

## 2.2 Alteração posterior no upstream

Em 2026-08-20, o upstream publicou:

`483a2203a8cc3a6f2c0da9070ca04b011f44b534`

Mensagem:

`fix warnings related to not using MAX constants`

No mesmo arquivo e na mesma função, o commit contém:

```diff
- let max_position = u64::max_value() as i128;
+ let max_position = u64::MAX as i128;
```

**Author date:** `2026-08-20T08:32:46Z`  
**Committer date:** `2026-08-20T08:43:26Z`

Intervalo entre o fechamento da #533 e o commit upstream:

`268 dias, 5 horas, 14 minutos e 21 segundos`.

## 2.3 Classificação

```text
same_file = true
same_function = true
same_removed_line = true
same_added_line = true
exact_text_match = true
source_predates_successor = true
explicit_reference_to_PR533_in_successor_commit = false
causal_derivation = TOKEN_VAZIO
```

### Conclusões permitidas

- a #533 contém anterioridade pública verificável dessa alteração;
- a mesma alteração literal entrou depois no upstream;
- a correspondência é linha-a-linha no mesmo locus.

### Conclusão não promovida

Não há prova suficiente, apenas dessa correspondência, para afirmar que o
commit `483a220…` foi produzido por consulta ou cópia da #533.

A mensagem do commit posterior registra como motivação local:
`fix warnings related to not using MAX constants`. Isso é uma razão
documentada no próprio commit e deve ser preservada como explicação concorrente,
sem inferir se houve ou não conhecimento da #533.

**Estado:** `VERIFIED_EXACT_SUCCESSOR_MATCH / CAUSALITY_TOKEN_VAZIO`

---

# 3. Controle canônico E-533-002 — Cygwin ocorreu depois, mas não veio da #533

O commit:

`890050cb9b89448a25e067349aed9734a3b03d5e`

foi integrado em `2025-12-09T19:38:21Z`, aproximadamente 14 dias e 16 horas
após o fechamento da #533.

Ele altera:

```c
#if !defined(_WIN32)
```

para:

```c
#if !defined(_WIN32) && !defined(__CYGWIN__)
```

em `c/blake3_impl.h`, além da guarda correspondente em
`c/blake3_dispatch.c`.

Entretanto, comparação direta mostra:

```text
PR533 base  308b95... : sem __CYGWIN__
PR533 head  15829f... : sem __CYGWIN__
890050cb...            : adiciona __CYGWIN__
```

Além disso:

```text
author date    = 2025-07-13T01:11:01Z
committer date = 2025-12-09T19:38:21Z
parent         = 308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
```

Ou seja, a alteração Cygwin já possuía autoria Git anterior à abertura da #533.

**Conclusão:**

```text
temporal_proximity_after_PR533 = VERIFIED
content_present_in_PR533 = false
direct_derivation_from_PR533 = REFUTED_IN_SCOPE
```

Esse controle é mantido deliberadamente para impedir que proximidade temporal
seja confundida com derivação.

---

# 4. E-533-003 — mesmo locus em src/platform.rs, solução posterior diferente

A #533 altera as funções:

- `words_from_le_bytes_32`
- `words_from_le_bytes_64`
- `le_bytes_from_words_32`
- `le_bytes_from_words_64`

simplificando offsets escritos como `0 * 4`, `1 * 4`, etc. para
`0`, `4`, `8`, etc.

Em 2026-08-20, o upstream publica:

`ba02549ebc81a9cce8eb1832b128c9b4cc8aa9c9`

Mensagem:

`Codex: remove arrayref`

Esse commit modifica as mesmas funções, mas com outra transformação:
remove `arrayref` e passa a usar slices + `try_into().unwrap()`.

Exemplo posterior:

```rust
u32::from_le_bytes(bytes[0 * 4..][..4].try_into().unwrap())
```

O HEAD upstream atual mantém essa solução posterior e não a forma proposta na
#533.

Classificação:

```text
same_file = true
same_functions = true
exact_hunk_match = false
implementation_equivalence = false
locus_recurrence = VERIFIED
causal_derivation = TOKEN_VAZIO
```

**Estado:** `VERIFIED_SAME_LOCUS_DIFFERENT_SOLUTION`

---

# 5. E-533-004 — benchmark/alinhamento: preocupação comum, implementação distinta

Na #533, `benches/bench.rs` propõe mecanismos de estabilidade de medição,
incluindo:

- `WARMUP_ITERATIONS`;
- cache warming;
- `test::black_box`;
- pré-alocação;
- discussão explícita de page/cache alignment bias;
- randomização de offsets.

Em 2026-09-10, o upstream mergeia a PR #582 no commit:

`6aab490a26124663329dfd3961b8469f8fdb158b`

que introduz `#[repr(align(64))]` em buffers de produção para remover bandas
de desempenho relacionadas a alinhamento.

As duas mudanças compartilham uma preocupação de engenharia com alinhamento e
variabilidade, mas:

- a #533 atua no harness de benchmark;
- #582 altera layout de objetos do código de produção;
- os patches não são iguais;
- os arquivos principais da alteração são diferentes.

Classificação:

```text
technical_theme_overlap = VERIFIED
exact_code_match = false
direct_derivation = TOKEN_VAZIO
```

**Estado:** `THEMATIC_OVERLAP_ONLY`

---

# 6. E-533-005 — seek/mmap #570 não é sucessor da mudança Seek da #533

A #533 altera `OutputReader::seek` em `src/lib.rs`, incluindo a modernização
`u64::MAX`.

A PR upstream #570, mergeada em:

`83b1746e7c42e5b3d228c81ba1ccb9b767dc5e2c`

altera outra operação: estabelecimento do comprimento para mmap por
`seek`/fallback em `src/io.rs` e chamadas relacionadas em `src/lib.rs`.

O próprio commit #570 registra:

> “Originally suggested by @nabijaczleweli in .../pull/487.”

Logo:

```text
word_seek_overlap = true
same_operation = false
same_hunk = false
documented_origin = PR487
derivation_from_PR533 = NOT_ESTABLISHED
```

**Estado:** `DISTINCT_CHANGE_WITH_DOCUMENTED_OTHER_SOURCE`

---

# 7. E-533-006 — mudanças da #533 que não estão no HEAD upstream atual

No upstream pin:

`6aab490a26124663329dfd3961b8469f8fdb158b`

foram verificadas ausências de propostas específicas da #533:

### benches/bench.rs

Não existe:

```rust
const WARMUP_ITERATIONS: usize = 3;
```

O upstream atual continua criando `Vec::new()` no benchmark relevante e mantém
o output local no corpo da iteração, em vez da forma de pré-alocação proposta
na #533.

### src/guts.rs

Não existe o método proposto:

```rust
pub fn is_empty(&self) -> bool
```

### src/hazmat.rs

O upstream atual mantém:

```rust
((input_len + 1) / 2).next_power_of_two()
```

e não a proposta da #533:

```rust
input_len.div_ceil(2).next_power_of_two()
```

Também mantém as referências na chamada de `parent_node_output`.

### src/lib.rs

O upstream atual mantém:

```rust
#[derive(Clone, Copy, Hash, Eq)]
pub struct Hash(...)
```

e não os `impl core::hash::Hash` manuais introduzidos na #533.

### c/blake3.h, c/blake3_impl.h, c/blake3_portable.c

A documentação/comentários extensivos introduzidos na #533 não foram
incorporados ao HEAD upstream observado.

### benches/README_CN.md

Esse arquivo da #533 não faz parte do conjunto observado no HEAD upstream.

Conclusão:

```text
PR533_entire_patch_absorbed = false
multiple_PR533_changes_absent_current_upstream = VERIFIED
```

Esse controle negativo é obrigatório para não selecionar apenas semelhanças.

---

# 8. E-589-001 — PR upstream #589 ainda não tem sucessor temporal observável

**PR:** `BLAKE3-team/BLAKE3#589`  
**Autor:** `rafaelmeloreisnovo`  
**Criada:** `2026-09-23T07:51:50Z`  
**Fechada:** `2026-09-23T07:59:18Z`  
**Merged:** `false`  
**Head SHA:** `809f6213674798cbcd0e76f3fbf060ebe1d68a1d`  
**Base SHA:** `6aab490a26124663329dfd3961b8469f8fdb158b`

O upstream HEAD observado em 2026-09-24 continua sendo
`6aab490a26124663329dfd3961b8469f8fdb158b`, commit de 2026-09-10.

Portanto, no snapshot atual não existe commit upstream posterior à #589 para
ser comparado como possível sucessor.

```text
post_589_upstream_successor = TOKEN_VAZIO
```

Nenhum claim de absorção posterior da #589 é permitido neste snapshot.

---

# 9. Precedentes internos do próprio BLAKE3: closed/unmerged não significa ausência

Os casos abaixo são usados somente para demonstrar uma propriedade objetiva do
workflow do projeto, não como prova sobre a #533.

## 9.1 #326 -> #342 -> commits diretos -> v1.5.0

A PR externa #326 foi fechada sem merge. O mantenedor abriu #342 sobre o
trabalho, e depois registrou publicamente:

`Landed as e0bb915... and cb32f0b...`

O trabalho saiu em v1.5.0.

**Fato:** uma contribuição pode chegar ao produto sem que a PR original esteja
marcada como merged.

## 9.2 #446 -> commit direto 2374b56...

A PR #446 propôs memory-mapped I/O em C. Durante a discussão, o mantenedor
implementou uma solução menor diretamente em `2374b56...`; o autor original
declarou que era suficiente e fechou sua PR.

**Fato:** uma necessidade proposta externamente pode ser reimplementada de
outra forma e a PR original permanecer unmerged.

## 9.3 #487 -> #570

A #570 declara explicitamente:

`Originally suggested by @nabijaczleweli in #487.`

**Fato:** o BLAKE3 registra pelo menos um caso em que uma ideia de PR externa
fechada reaparece em PR/commit posterior do mantenedor com crédito explícito.

Esses precedentes justificam metodologicamente analisar `closed/unmerged`
por linhagem de ideia/código, e não apenas pelo campo `merged`.

---

# 10. Resultado probatório canônico deste snapshot

## PROVADO

1. A #533 existiu publicamente antes dos commits upstream posteriores aqui
   examinados.
2. A #533 foi fechada sem merge.
3. `u64::MAX` aparece na #533 e depois entra no upstream como correspondência
   literal no mesmo arquivo/função.
4. O upstream não referencia a #533 na mensagem do commit `483a220...`.
5. A correção Cygwin foi integrada pouco depois do fechamento da #533, mas sua
   autoria Git é anterior e o conteúdo não aparece no head da #533.
6. `src/platform.rs` foi revisitado depois nas mesmas funções por um commit
   chamado `Codex: remove arrayref`, com solução diferente.
7. Várias propostas técnicas da #533 continuam ausentes do upstream atual.
8. O próprio BLAKE3 possui precedentes documentados de PRs `closed/unmerged`
   cujo trabalho/requisito reaparece por outra PR, commit direto ou redesign.
9. A #589 ainda não possui janela temporal posterior observável no upstream.

## EVIDÊNCIA DE RECORRÊNCIA TÉCNICA, SEM CLAIM DE INTENÇÃO

Existe recorrência demonstrável de:

```text
PR externa / proposta
-> fechamento ou substituição
-> mesmo locus ou requisito posteriormente modificado
```

em diferentes casos BLAKE3.

Para a #533 especificamente, o item mais forte é
`E-533-001 / u64::MAX`.

## TOKEN_VAZIO / NÃO ESTABELECIDO

Permanecem não estabelecidos neste snapshot:

- que `483a220...` foi causalmente derivado da #533;
- que o fechamento da #533 foi destinado a retirar crédito;
- que existe uma política deliberada para manter referência exclusiva;
- que houve apropriação ilícita;
- que bots foram usados para ocultar origem;
- que textos foram alterados para fabricar justificativa ex post.

Essas hipóteses só podem ser promovidas com evidência independente.

---

# 11. Regra para próximos achados

Cada futuro caso deverá conter:

```text
source_pr
source_sha
source_timestamp
source_file
source_function
source_removed_hunk
source_added_hunk
successor_pr_or_commit
successor_sha
successor_timestamp
delta_time
same_file
same_function
exact_text_match
normalized_match
semantic_locus_match
explicit_credit
documented_alternative_source
current_head_presence
causal_status
limitations
```

Escala:

```text
EXACT = linha/hunk literal
STRUCTURAL = mesma transformação estrutural
LOCUS = mesma função/superfície, solução diferente
THEME = mesma preocupação, código distinto
NONE = sem relação material localizada
```

Nenhuma categoria acima, isoladamente, equivale a conclusão jurídica.

---

# 12. Invariante final

O objetivo desta custódia não é tornar uma hipótese mais forte do que a prova.
É impedir que a prova seja mais fraca do que o registro permite.

```text
ANTERIORIDADE pode ser provada por timestamp + conteúdo.
IDENTIDADE pode ser provada por diff/hunk.
ORDEM pode ser provada pela timeline.
CAUSALIDADE requer ponte adicional.
INTENÇÃO requer prova própria.
ILICITUDE requer enquadramento e processo competente.
```

**Assinatura autoral da compilação:** RAFCODE-Φ / ΔRafaelVerboΩ  
**Regra epistêmica:** símbolo não substitui medida; hipótese não substitui prova;
ausência preservada é `TOKEN_VAZIO`.
