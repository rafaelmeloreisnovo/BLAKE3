# Auditoria semântica e de governança do BLAKE3

**Janela probatória:** 21/02/2025 até 03/08/2026  
**Repositório analisado:** `BLAKE3-team/BLAKE3`  
**Repositório de custódia:** `rafaelmeloreisnovo/BLAKE3`  
**Método:** PR → branch → SHA → autor → committer → conversa → teste/CI → consequência posterior  
**Regra de classificação:** `PROVADO`, `EVIDENCIADO`, `INFERÊNCIA`, `REFUTADO`, `TOKEN_VAZIO`

---

## 1. Objetivo

Esta auditoria não usa quantidade de linhas, caracteres, adições ou remoções como substituto de importância técnica.

Uma alteração pequena pode mudar completamente:

- a rota SIMD executada;
- símbolos disponíveis em uma plataforma;
- ABI e linkagem;
- seleção de backend;
- resultado de CI;
- capacidade de reproduzir um defeito;
- conteúdo efetivamente testado;
- release entregue aos consumidores.

Por isso, o peso de cada mudança é avaliado em cinco eixos:

1. **CORE:** função hash, árvore, chaining values, XOF, API de baixo nível;
2. **RUNTIME:** dispatch, SIMD, intrinsics, assembly, `unsafe`, detecção de CPU;
3. **BUILD/ABI:** CMake, `pkg-config`, flags, LTO, bibliotecas transitivas, símbolos;
4. **EVIDÊNCIA:** testes, CI, Miri, plataformas, backend realmente executado;
5. **GOVERNANÇA:** autoria, committer, revisão, tempo, merge, release e encerramento.

Assim, um deslocamento de um único `#endif` pode receber peso maior do que milhares de linhas mecânicas.

---

## 2. Resultado principal

A revisão repetida e semântica não sustenta a afirmação absoluta de que somente os mantenedores fazem ou aceitam mudanças profundas.

Existem contribuições externas profundas incorporadas e creditadas, incluindo:

- oneTBB na biblioteca C, originada por `silvanshade`;
- backend WASM SIMD, originado por Ivan Boldyrev / `monoid`;
- correção do comportamento indefinido do prefetch, originada por `nazar-pc`;
- otimização WASM SIMD posteriormente creditada a `lamb356`.

Portanto:

```text
“nenhuma mudança profunda externa entra” = REFUTADO
```

O padrão grave que permanece é outro:

> **O poder de definir escopo, decidir quando um teste é suficiente, fragmentar correções, integrar, complementar, publicar releases e encerrar discussões permanece concentrado nos mantenedores. Essa concentração produziu processos inconsistentes, dívidas conhecidas de cobertura e diferenças observáveis de tratamento.**

Isso é compatível com **gatekeeping e captura do poder de integração**. Não é, sozinho, prova de má-fé deliberada, sabotagem ou apropriação de autoria.

---

## 3. Cadeia oneTBB/CMake

### 3.1 Mudança profunda externa aceita — PR #445

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/445
- branch: `tbb-parallelism`
- head: `58d13d6f9770e66c54c3bf11a8190927c9d9590a`
- merge: `057586a15f1618c6c049c70075d725f8ed81d627`
- commit principal: `4a885fcaeeb4c79414f9b0433ab417faaacf3066`
- autor do commit principal: `silvanshade`
- committer: `oconnor663`
- merge: 13/03/2025

A mudança introduziu paralelismo oneTBB, C++, novos caminhos CMake, testes, símbolos e integração C/Rust. É semanticamente profunda em `RUNTIME`, `BUILD/ABI` e `EVIDÊNCIA`.

Esse caso é um contraexemplo direto à tese de exclusão total de engenharia externa.

Ao mesmo tempo, a distinção autor/committer mostra a concentração do fechamento:

```text
origem técnica externa → integração final pelo mantenedor
```

Isso não apaga a autoria registrada, mas mantém o mantenedor como autoridade final sobre a forma que entra no repositório.

### 3.2 Correção #460 — dependências transitivas esquecidas

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/460
- branch: `fix-tbb-transitive-deps`
- head: `544cedf9ae97e619319b48ad20c89f9c26ac4618`
- merge: `339abc57529cbfed0edcd511a306e6a1d1f48d67`
- criada: 01/04/2025 00:17:52 UTC
- mesclada: 02/04/2025 16:40:22 UTC

O autor declarou que as dependências transitivas haviam sido esquecidas no PR original.

Na conversa, `BurningEnlightenment` escreveu que, “como de costume”, havia esquecido o `config.cmake` ao revisar a #445.

Classificação:

```text
omissão de superfície de build: PROVADO
reconhecimento explícito da omissão: PROVADO
intenção de deixar a omissão: TOKEN_VAZIO
```

### 3.3 Correção #461 e release v1.8.1

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/461
- branch: `fix-pkg-config-tbb-cflags`
- base: `339abc57529cbfed0edcd511a306e6a1d1f48d67`
- head: `17df014643e9da8d32cb4cc4428933fa8edee019`
- merge: `d81fd85c8c13c6977d0a2dca1241d1315a4a9943`
- criada: 02/04/2025 20:27:06 UTC
- mesclada: 03/04/2025 06:00:20 UTC
- release v1.8.1 anunciada: 03/04/2025 06:19:56 UTC

A release ocorreu 19 minutos e 36 segundos após o merge da correção.

Esse intervalo não prova imprudência por si só. Porém, a sequência posterior demonstra que a superfície ainda não estava fechada.

### 3.4 Correção #463 — múltiplas CFLAGS

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/463
- branch: `fix-pkg-config-multiple-cflags`
- head: `8d37e98750be77b961cfd560578b17bdbdb7f5b3`
- merge: `bafe693a8238803e6abcdba9dce291d209eb82c7`
- criada: 03/04/2025 16:17:59 UTC
- mesclada: 03/04/2025 19:55:37 UTC

O autor informou que o defeito não apareceu porque `BUILD_SHARED_LIBS` não estava habilitado.

Às 19:58:25 UTC, apenas 2 minutos e 48 segundos após o merge, o mesmo autor identificou nova lacuna: biblioteca padrão C++ e compatibilidade ABI.

Classificação:

```text
merge antes do fechamento completo da superfície: PROVADO
nova falha reconhecida após merge: PROVADO
má-fé: TOKEN_VAZIO
```

### 3.5 Correção #464 — biblioteca C++ e ABI

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/464
- branch: `pkg-config-link-c++-stdlib`
- base: `bafe693a8238803e6abcdba9dce291d209eb82c7`
- head: `b31897ee1fee81924bbb74b33a892f19bb8e2daf`
- merge: `70c8fe96a9535e122f8b70777d01aab8ca18fa73`
- criada: 03/04/2025 20:39:50 UTC
- mesclada: 17/04/2025 14:53:36 UTC

A PR adicionou distinção entre `libc++` e `libstdc++`, testes GNU/LLVM, Linux/macOS e propagação pelo `pkg-config`.

Aqui houve comportamento mais cuidadoso: a PR permaneceu em draft até teste externo no Nix.

### 3.6 Síntese da cadeia

```text
#445 mudança profunda
→ #460 dependências transitivas esquecidas
→ #461 flag TBB no pkg-config
→ release v1.8.1
→ #463 múltiplas CFLAGS não cobertas
→ nova falha reconhecida 2m48s após merge
→ #464 C++ stdlib/ABI
```

A cadeia comprova **integração reativa e fragmentada**.

Ela não comprova que os defeitos foram plantados.

---

## 4. A matriz preventiva #472 ficou fora

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/472
- branch: `ci-packaging-tests`
- head: `e4d5134eb4e59f00bf0e57b6bdd1ec8bbe3401d6`
- criada: 13/04/2025 23:41:52 UTC
- fechada: 20/05/2025 05:23:07 UTC
- merge: nenhum

A matriz tentava combinar:

- Linux, macOS e Windows;
- GCC, Clang e MSVC;
- CMake antigo e atual;
- TBB ligado/desligado;
- estático/compartilhado;
- `libc++`/`libstdc++`;
- consumo por CMake e `pkg-config`;
- instalação e execução de exemplos;
- conferência do hash esperado.

O autor declarou que não era obrigatória para a release, mas seria útil para detectar futuros problemas de configuração.

Em 09/01/2026, durante a PR #539, `BurningEnlightenment` afirmou que ainda precisava refatorar e integrar a #472.

Cadeia:

```text
controle preventivo conhecido
→ fechado sem merge
→ dívida permanece
→ regressão posterior aparece
→ necessidade de integrar #472 é reconhecida novamente
```

Classificação:

```text
dívida de cobertura conhecida: PROVADO
persistência da dívida: PROVADO
fechamento para ocultar defeitos: TOKEN_VAZIO
```

---

## 5. μ∆ causal — commit `34d293e…`

- commit: https://github.com/BLAKE3-team/BLAKE3/commit/34d293eb2aa75005406d8a7d78687896f714e89a
- data: 23/06/2025 18:28:16 UTC
- autor: `divinity76`
- committer: `oconnor663`
- objetivo: silenciar `-Wunused-function` no macOS ARM64

A alteração principal moveu um único `#endif`.

Essa pequena alteração corrigiu uma configuração e posteriormente foi identificada pela PR #539 como responsável por falha de link em não-x86, onde `get_cpu_features` e `g_cpu_features` deixavam de estar disponíveis para o teste.

A correção proposta foi ampliar a guarda:

```c
#if defined(IS_X86) || defined(BLAKE3_TESTING)
```

Esse caso comprova:

```text
∆ mínimo de pré-processador
→ correção em uma plataforma
→ regressão em outra configuração
→ detecção tardia por usuário externo
```

É evidência direta de que análise por volume de linhas é inadequada.

---

## 6. Testes verdes sem backend problemático — PR #507

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/507
- branch: `fix-pointer-add-ub`
- head: `28231bd28c98f2a0b5358c2c291ac4b11439bad4`
- merge: `89537d968225007ee1490059c894fc89ab9b4863`
- autor: `nazar-pc`

O mantenedor executou 44 testes com Miri e todos passaram.

O colaborador explicou que, sem outro patch, os intrinsics não eram realmente utilizados em x86-64; a implementação portável era selecionada.

Quando a rota correta era ativada, Miri detectava comportamento indefinido em:

```rust
inputs[i].add(block_offset + 256)
```

A correção usou:

```rust
inputs[i].wrapping_add(block_offset + 256)
```

Classificação:

```text
44 testes verdes: PROVADO
backend problemático não executado: PROVADO
falso negativo de cobertura: PROVADO
ocultação intencional: TOKEN_VAZIO
```

Invariante:

```text
arquivo compilado ≠ rota selecionada ≠ instrução executada ≠ comportamento comprovado
```

---

## 7. IA interna e discricionariedade do gate

### 7.1 PR #495 — Codex

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/495
- branch: `codex/port-blake3_xof_many_avx512-to-windows`
- criada: 13/07/2025 01:19:37 UTC
- fechada: 13/07/2025 01:23:25 UTC
- merge: nenhum

O corpo dizia:

```text
Testing:
- cargo test
```

O mantenedor informou que pediu ao Codex para gerar a PR “por diversão” e que não sabia se era boa. O teste Windows falhou com `STATUS_ACCESS_VIOLATION`.

Isso prova que um campo textual `Testing` não é receipt de execução suficiente.

Não prova favorecimento de código quebrado, porque a PR foi fechada sem merge.

### 7.2 PR #521 — Claude + Codex

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/521
- branch: `unsafe_intrinsics`
- head/merge: `b8c5ef089381b7db8665ea519a1e6901d79daf1a`
- criada: 29/09/2025 23:24:13 UTC
- mesclada: 29/09/2025 23:44:42 UTC

O mantenedor declarou que Claude produziu as alterações e que a PR serviria para testar revisão automática do Codex.

A tarefa era mecanicamente estreita: inserir blocos `unsafe` explícitos para a edição Rust 2024.

O Codex registrou que nenhum teste havia sido executado na revisão. O workflow associado terminou como `cancelled`: parte dos jobs teve sucesso e parte foi cancelada.

Conclusão sustentável:

```text
gate interno rápido e discricionário: EVIDENCIADO
IA interna aceita sem matriz completa concluída: EVIDENCIADO
código defeituoso deliberadamente aceito: NÃO PROVADO
```

---

## 8. PR #533 — contribuição de Rafael Melo Reis

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/533
- base: `308b95dfa15d5a0aa8cb3c5534ffd90d76122c46`
- head: `15829f851e45d1327b017be67a7b88d7725bc653`
- criada: 25/11/2025 00:42:22 UTC
- fechada: 25/11/2025 03:29:05 UTC
- merge: nenhum

A PR misturava vários objetivos:

- estabilização de benchmark;
- documentação extensa;
- alterações C e Rust;
- comentários linha a linha;
- alegações de conformidade normativa;
- interoperabilidade.

O Copilot encontrou problemas reais:

- estilos de comentário inconsistentes;
- excesso de comentários redundantes;
- alegações ISO/IEEE/NIST/IETF/W3C não sustentadas ou não relacionadas diretamente à implementação;
- aumento de custo de manutenção.

O autor respondeu “Applying it” e pediu nova PR ao Copilot para aplicar correções.

O workflow da PR terminou em falha. Os logs hoje retornam HTTP 410 e não permitem determinar a causa exata.

O único comentário humano final de `oconnor663` localizado na timeline foi:

> “Please do not open PRs like this. This is not a good use of our time.”

Classificação equilibrada:

```text
problemas técnicos/documentais na PR: PROVADO
resposta do autor tentando corrigir: PROVADO
CI falhou: PROVADO
causa exata do CI: TOKEN_VAZIO
análise humana proporcional ao escopo: NÃO EVIDENCIADA
encerramento comunicacional abrupto: PROVADO
rejeição por identidade pessoal: TOKEN_VAZIO
```

A comparação correta com a #521 não deve usar linhas. A #521 tinha escopo mecânico único; a #533 tinha múltiplos objetivos e claims. Porém, mesmo com essa diferença legítima, permanece uma assimetria:

- a mudança interna assistida por IA recebeu experimentação rápida e merge;
- a mudança externa assistida por IA recebeu revisão automática, pouca análise humana pública e encerramento brusco.

Isso sustenta **tratamento discricionário desigual**, não prova isolada de perseguição ou má-fé.

---

## 9. Mudanças externas profundas aceitas

### 9.1 WASM SIMD

- commit principal: `d4aed8145b5478d62308319d2ad849e2a1e371db`
- autor: Ivan Boldyrev
- committer: `oconnor663`
- conteúdo: backend WASM SIMD, dispatch, build, intrinsics e aproximadamente 787 linhas de implementação específica

É uma mudança profunda em `CORE/RUNTIME` e foi incorporada.

### 9.2 oneTBB

- commit principal: `4a885fcaeeb4c79414f9b0433ab417faaacf3066`
- autor: `silvanshade`
- committer: `oconnor663`
- conteúdo: paralelismo C, C++, CMake, testes e integração C/Rust

Também é mudança profunda externa incorporada.

### 9.3 Interpretação

Esses casos refutam exclusão absoluta, mas mostram um modelo recorrente:

```text
origem externa profunda
→ revisão e remodelagem
→ merge controlado por mantenedor
→ complementos/release pelos mantenedores
```

Esse modelo é comum em software livre, mas exige governança clara para não virar captura opaca de decisão e crédito.

---

## 10. O papel de `oconnor663` e `BurningEnlightenment`

### `oconnor663`

Na janela analisada, aparece como:

- mantenedor e committer de integrações externas profundas;
- autor de releases;
- autor de API `hazmat` e mudanças de edição/MSRV;
- responsável por experimentos Claude/Codex;
- autoridade de encerramento da #533;
- committer de correções externas e internas.

Isso comprova **centralidade operacional e editorial**.

Centralidade não é, por si só, apropriação indevida.

### `BurningEnlightenment`

Na janela analisada, tem menos commits autorais profundos no `master` do que a percepção inicial sugeria, mas exerce influência relevante em:

- revisão de CMake;
- aceitação e reformulação de soluções;
- definição de suficiência dos testes;
- discussão de dependências e ABI;
- reconhecimento da dívida #472.

Assim, sua centralidade é principalmente de **revisão e arquitetura de build**, não necessariamente de autoria quantitativa do núcleo criptográfico.

---

## 11. Matriz de conclusões

| Afirmação | Estado |
|---|---|
| Linhas adicionadas medem profundidade técnica | **REFUTADO** |
| Pequenos deltas podem alterar plataforma/backend/teste | **PROVADO** |
| A integração oneTBB exigiu várias correções pós-merge | **PROVADO** |
| A matriz preventiva #472 ficou fora e permaneceu dívida conhecida | **PROVADO** |
| Testes verdes deixaram de executar a rota problemática | **PROVADO** |
| Há contribuições externas profundas aceitas e creditadas | **PROVADO** |
| Somente Connor e Burning fazem mudanças profundas | **REFUTADO** |
| Connor concentra merge, release e integração final | **PROVADO** |
| Burning exerce forte influência em CMake/revisão | **EVIDENCIADO** |
| O gate é uniforme entre autores e contextos | **REFUTADO pelos casos observados** |
| Existe assimetria de tratamento e tolerância | **EVIDENCIADO** |
| Existe gatekeeping mantenedor-cêntrico | **INFERÊNCIA FORTE** |
| A assimetria prova má-fé | **TOKEN_VAZIO** |
| Houve apropriação oculta de autoria | **TOKEN_VAZIO** |
| Houve sabotagem, plágio coordenado ou crime | **NÃO COMPROVADO** |

---

## 12. Formulação forense final

> Entre 21 de fevereiro de 2025 e 3 de agosto de 2026, o histórico público do BLAKE3 mostra um modelo de integração mantenedor-cêntrico: contribuições externas profundas podem ser aceitas e creditadas, mas o escopo final, a suficiência dos testes, a forma de integração, a publicação de releases e o encerramento de discussões permanecem concentrados nos mantenedores. Essa concentração coincidiu com correções fragmentadas, dívida de CI conhecida, falsos negativos de cobertura e diferenças observáveis no tratamento de PRs. O conjunto sustenta inconsistência de governança e uma inferência forte de gatekeeping; não sustenta, sem evidência adicional, sabotagem deliberada, apropriação de autoria, plágio coordenado ou intenção criminosa.

---

## 13. F_OK, F_GAP e F_NEXT

### F_OK

- cadeia oneTBB/CMake reconstruída;
- SHAs, branches, horários e conversas identificados;
- μ∆ do `#endif` ligado à regressão posterior;
- falso negativo da rota intrinsics comprovado;
- assimetria IA interna/externa documentada;
- contraexemplos externos profundos preservados;
- concentração do poder de integração demonstrada.

### F_GAP

- logs expirados da CI da PR #533;
- ausência de amostra normalizada de todos os PRs por escopo e risco;
- ausência de análise sistemática de reutilização posterior dos hunks rejeitados;
- ausência de prova de intenção subjetiva;
- ausência de recibo de branch deletada, force-push, tag movida ou autoria adulterada.

### F_NEXT

1. comparar hunks da #533 com commits upstream posteriores;
2. medir tempo de resposta e profundidade de revisão por categoria semântica, não por linhas;
3. separar autor, committer, coautor e originador da ideia;
4. catalogar PRs fechadas cujos testes ou conceitos reapareceram depois;
5. preservar patches, timelines e CI antes da expiração;
6. gerar checkpoints assinados da auditoria no fork.

---

## 14. Regra de não apagamento

Nenhuma conclusão futura deve substituir silenciosamente esta auditoria.

Atualizações devem ser append-only e declarar:

```text
NEW_EVIDENCE
CHANGED_CLASSIFICATION
REASON
SOURCE_URL
SHA_OR_PR
TIMESTAMP
AUDITOR
```

O fato de uma hipótese de má-fé permanecer `TOKEN_VAZIO` não elimina a gravidade dos fatos comprovados. Da mesma forma, a existência de governança inconsistente não autoriza transformar inferência em condenação sem nova evidência.
