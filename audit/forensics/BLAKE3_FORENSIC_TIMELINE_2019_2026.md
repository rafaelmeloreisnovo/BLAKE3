# BLAKE3 — Linha do tempo forense 2019–2026

**Snapshot verificado em:** `2026-08-03T08:41:00-03:00`  
**Upstream:** https://github.com/BLAKE3-team/BLAKE3  
**Fork de custódia:** https://github.com/rafaelmeloreisnovo/BLAKE3

## 2019

### 2019-12-07 — assert incorreto corrigido

- Commit: `0cb4f6dd2cadbe9fa03a1eabeb3359d3bc66ebcc`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/0cb4f6dd2cadbe9fa03a1eabeb3359d3bc66ebcc
- Registro: o assert disparava incorretamente quando `MAX_SIMD_DEGREE=1`.
- Classe: `PROVADO`.
- Intenção de introduzir erro: `TOKEN_VAZIO`.

## 2020

### 2020-01-19 — prefetch manual introduzido

- Commit: `b8c33e11ef4a85a0d88743cb7f00b66c2c9fc538`
- Autor/committer público: `sneves`.
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/b8c33e11ef4a85a0d88743cb7f00b66c2c9fc538
- Alteração: `_mm_prefetch(inputs[i].add(block_offset + 256)...)` em rotas SIMD.
- Consequência descoberta em 2025: formar ponteiro fora da alocação com `.add()` podia resultar em comportamento indefinido segundo o modelo de memória Rust.
- Classe da introdução: `PROVADO`.
- Dolo: `TOKEN_VAZIO`.

### 2020-01-20/21 — lazy merge e crescimento estrutural

- Commit: `67262dff31461d3fb801f3fe01382abb77387735`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/67262dff31461d3fb801f3fe01382abb77387735
- Alteração: duplicação do tamanho máximo de subárvore incremental e mudança do invariante de divisão.
- Relação posterior: um binding de teste manteve `cv_stack` com 1728 bytes, enquanto o layout real passou a exigir 1760.
- Classe da mudança principal: `PROVADO`.
- Conhecimento prévio da divergência no binding: `TOKEN_VAZIO`.

### 2020-01-22 — release 0.1.3 registra prefetch

- Commit: `e17c45ddd54bb80fe8a3a2ea384ba87e7ce1dff1`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/e17c45ddd54bb80fe8a3a2ea384ba87e7ce1dff1
- Release notes: prefetch em implementações x86 e outras melhorias.
- A relevância forense é temporal: o comportamento só foi corrigido em 2025.

## 2021

### 2021-02-06 — “revert unwanted changes”

- Commit: `8c350836b81477a5ea49f14e5ca636f1eb1102f5`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/8c350836b81477a5ea49f14e5ca636f1eb1102f5
- O título é genérico e exige diff/parent para determinar exatamente o que foi considerado indesejado.
- Classe: existência da reversão `PROVADO`; motivação completa `TOKEN_VAZIO`.

### 2021-07-26 — PR nº 185 abre recorrência Clippy

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/185
- Autor: `lrazovic`.
- Criada: `2021-07-26T17:20:05Z`.
- Fechada sem merge: `2022-03-10T07:49:31Z`.
- Sugestões:
  - `ChunkState::is_empty`;
  - remover `derive(Hash)`;
  - remover borrow desnecessário;
  - padrões de caractere único.
- O mantenedor rejeitou `is_empty` e a remoção de `Hash`, aceitou conceitualmente a limpeza de borrow e alertou para falsos positivos do Clippy.
- Classe: `PROVADO`.

### 2021-11-05 — saída incorreta e UB em SSE2/Windows

- Issue: https://github.com/BLAKE3-team/BLAKE3/issues/206
- Correção: `371b5483c95be1e0250c5209d68a8536406152de`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/371b5483c95be1e0250c5209d68a8536406152de
- Defeito: uso de `xmm10`, registrador callee-save no ABI Windows, sem preservação adequada.
- Efeito reproduzido: hash incorreto e comportamento indefinido em configuração SSE2 específica.
- Release de segurança: `c61c663ec5dc581a449ed03a69397c698efc4cb5`.
- A release declara que o erro existia desde o suporte SSE2 na versão 0.3.7.
- Classe: `PROVADO`.
- Introdução deliberada: `TOKEN_VAZIO`.

## 2022

### 2022-11-23/26 — saída incorreta em AVX-512/GCC antigo

- Correção: `342f9f8067aa08b65e94d34710a417a498de66a8`
- Release: `67e4d04a3c025288ecb1276800d435577e2008bd`
- Issue: https://github.com/BLAKE3-team/BLAKE3/issues/271
- Defeito: `_mm512_cmp_epu32_mask` quebrado em GCC 5.4/6.1 podia produzir saída incorreta em debug usando intrinsics.
- Classe: `PROVADO`.
- Impacto público concreto: não quantificado; `TOKEN_VAZIO`.

## 2023

### 2023-02-04 — comentário removido por ser considerado incorreto

- Commit: `4c819d01bcea974bbac8199dafe9ab68a24f796b`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/4c819d01bcea974bbac8199dafe9ab68a24f796b
- O autor declarou que parte do comentário de `check_one_checkfile` parecia incorreta e o removeu.
- Classe: revisão documental `PROVADO`; falsificação deliberada do comentário anterior `NÃO DEMONSTRADA`.

## 2024

### 2024-07-14 — serialização supostamente compatível revertida

- Reversão: `43ce639d136a41289595749710212ef4f67f5f7f`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/43ce639d136a41289595749710212ef4f67f5f7f
- Issue: https://github.com/BLAKE3-team/BLAKE3/issues/414
- Release: `acaeabeff3e051d5396e91732b1fd2f0b2641402`
- Fato: serializar `Hash` com `serde_bytes` era compatível em formatos autodescritivos, mas quebrava compatibilidade em formatos como `bincode`.
- O próprio commit classifica a mudança como erro de retrocompatibilidade.
- Classe: afirmação técnica anterior desmentida `PROVADO`.
- Conhecimento prévio/engano deliberado: `TOKEN_VAZIO`.

### 2024-08-19 — `blake3_guts` removido

- Commit: `919091615b1093c2096129646b21d9103193a30b`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/919091615b1093c2096129646b21d9103193a30b
- Conteúdo removido:
  - crate experimental;
  - testes de CI;
  - API de compressão, chunks, parents, XOF, XOF-XOR e universal hash;
  - plano para RISC-V, ARM SVE, Wasm SIMD, Bao, Bessie e BLAKE3-AEAD.
- Motivo declarado: código experimental sem desenvolvimento posterior e risco de manter código não utilizado.
- Classe: arquitetura abortada e reconstruível `PROVADO`.
- Ocultação forense: `REFUTADO` no sentido estrito, pois o commit de exclusão é público.
- Perda operacional ao olhar somente o master: `PROVADO`.

## 2025

### 2025-02-20 — `unsafe extern "C"` revertido por MSRV

- Commit: `2cb802b115aa248f6534f353f26cc9060378bc31`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/2cb802b115aa248f6534f353f26cc9060378bc31
- Motivo declarado: mudança necessária para Rust 2024, porém naquele momento elevava desnecessariamente a MSRV.
- Classe: reversão de compatibilidade `PROVADO`.

### 2025-07-13 — Codex produz PR com access violation

- PR nº 495: https://github.com/BLAKE3-team/BLAKE3/pull/495
- Criada: `2025-07-13T01:19:37Z`.
- Fechada sem merge: `2025-07-13T01:23:25Z`.
- Branch: `codex/port-blake3_xof_many_avx512-to-windows`.
- O mantenedor declarou que o Codex gerou a PR e que não sabia se era boa.
- O teste Windows terminou com `STATUS_ACCESS_VIOLATION`.
- Classe: código de IA defeituoso `PROVADO`; entrada no master `NÃO`.

### 2025-08-12/13 — prefetch corrigido após Miri

- PR nº 507: https://github.com/BLAKE3-team/BLAKE3/pull/507
- Autor: `nazar-pc`.
- Merge commit: `89537d968225007ee1490059c894fc89ab9b4863`.
- Correção: `.add()` → `.wrapping_add()` para o ponteiro de prefetch.
- O mantenedor inicialmente não reproduziu porque a configuração padrão não exercitava a implementação intrinsics; o colaborador explicou o patch necessário.
- Classe: UB real e lacuna de cobertura `PROVADO/EVIDENCIADO`.

### 2025-09-15 — alerta de antivírus 2/72

- Issue nº 517: https://github.com/BLAKE3-team/BLAKE3/issues/517
- Criada: `2025-09-15T09:27:43Z`.
- Fechada: `2026-01-09T21:25:46Z`.
- Relato: 2 de 72 mecanismos sinalizaram o binário Windows 1.8.2.
- Comentário posterior: 1.8.3 não apresentava o mesmo alerta.
- Classe: alerta isolado `PROVADO`; malware/botnet `NÃO DEMONSTRADO`.
- Gap: não foi localizado no issue um laudo binário reproduzível completo; `TOKEN_VAZIO`.

### 2025-09-29 — Claude gera alteração mesclada; Codex revisa

- PR nº 521: https://github.com/BLAKE3-team/BLAKE3/pull/521
- Título: `add explicit unsafe blocks to more unsafe functions`.
- O corpo declara uso de Claude e finalidade de testar revisão automatizada do Codex.
- Estado: mesclada.
- Classe: IA declarada em contribuição interna `PROVADO`.

### 2025-11-12 — pin de dependência revertido

- Commit: `eae9bf376a1c4797df7be6e49e735c0a5d91dcb0`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/eae9bf376a1c4797df7be6e49e735c0a5d91dcb0
- Motivo: upstream `crypto-common` passou a fazer o pin.
- Classe: reversão de dependência `PROVADO`.

### 2025-11-25 — PR nº 533 de Rafael

- PR: https://github.com/BLAKE3-team/BLAKE3/pull/533
- Autor: `rafaelmeloreisnovo`.
- Criada: `2025-11-25T00:42:22Z`.
- Fechada sem merge: `2025-11-25T03:29:05Z`.
- 24 commits; 10 arquivos; 1566 adições; 195 remoções.
- Conteúdo incluía benchmark, documentação, comentários, mudanças Clippy e C/Rust.
- Encerramento do mantenedor: “Please do not open PRs like this. This is not a good use of our time.”
- Classe:
  - encerramento abrupto `PROVADO`;
  - problemas de escopo e qualidade `EVIDENCIADO`;
  - tratamento comunicacional diferente de outras PRs `EVIDENCIADO`;
  - apropriação posterior de trechos `NÃO EVIDENCIADA`;
  - discriminação ou crime `NÃO DEMONSTRADO`.

## 2026

### 2026-01-09 — alerta VirusTotal encerrado

- Issue nº 517 foi encerrado como `not_planned` após referência à versão 1.8.3 sem alerta.
- Isso não equivale a auditoria de supply chain completa.
- Classe: encerramento público `PROVADO`; ausência de malware `NÃO PODE SER PROVADA somente por esse issue`.

### 2026-03-06 — Claude encontra divergência de `cv_stack`

- Commit: `6eebbbd67935cc6b8fe24da164ae968a2e3973cb`
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/6eebbbd67935cc6b8fe24da164ae968a2e3973cb
- Correção: `1728` → `1760` bytes em binding C/Rust de teste.
- Mensagem: o tamanho antigo era correto antes do crescimento da pilha; input quase máximo seria necessário para atingir o fim; Claude detectou em auditoria.
- Classe: divergência estrutural e detecção tardia `PROVADO`.
- Exploração real: `TOKEN_VAZIO`.

### 2026-04-24/25 — LTO quebra build ARM e novo gate é adicionado

- Fix: `299b1e209ef80fd2261567635198331f683c4831`
- CI: `6a45feedc618e45d3e3fd8430887728352295b73`
- Release 1.8.5: `93a431c78a52d7ccf0f366f106467f5070e6075e`
- Issue: https://github.com/BLAKE3-team/BLAKE3/issues/550
- Classe: build break e cobertura adicionada após falha `PROVADO`.

### 2026-05-21 — último commit upstream visível no corte

- Commit: `8aa5145039b972ba30e98e788752d37d14568824`
- Data: `2026-05-21T23:53:04Z`.
- Link: https://github.com/BLAKE3-team/BLAKE3/commit/8aa5145039b972ba30e98e788752d37d14568824
- Observação: “último visível” significa o resultado retornado no corte desta auditoria; não implica inatividade futura.

### 2026-08-03 — criação deste snapshot forense

- Fork master observado antes da branch: `a129db273e47feed54b7ad79c2aa2a15a9bb2d98`.
- Branch de auditoria: `audit/blake3-forensics-20260803`.
- Regra: preservar links, SHAs, datas, claims e gaps sem promover hipótese a fato.

## Síntese temporal

```text
2019–2026:
erros de assert, ABI, SIMD, ponteiros, serialização, layout e build
→ descobertos por CI incomum, Miri, auditoria humana ou IA
→ corrigidos/revertidos em commits públicos
→ alguns atravessaram anos ou releases
```

O padrão é compatível com complexidade, duplicação de backends e cobertura incompleta. A hipótese de introdução deliberada exige evidência adicional de intenção, conhecimento prévio, ocultação e benefício.
