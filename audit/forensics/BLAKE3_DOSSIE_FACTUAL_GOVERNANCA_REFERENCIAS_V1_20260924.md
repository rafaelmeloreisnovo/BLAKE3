# BLAKE3 — Dossiê Factual de Governança, Integração, Security Wording e Referências V1

**Data:** 2026-09-24  
**Repositório de custódia:** rafaelmeloreisnovo/BLAKE3  
**Fonte pública principal:** BLAKE3-team/BLAKE3  
**Classe:** FACTUAL_DOSSIER / NON_ACCUSATORY / APPEND_ONLY / FAIL_CLOSED  
**Regra:** descrever fatos observáveis; não atribuir intenção, fraude, plágio, supressão, conluio ou hierarquia informal sem evidência direta.

## 1. Finalidade

Este dossiê reúne, em forma narrativa, fatos já documentados no Google Drive e no GitHub sobre:

- concentração observada de atividade de integração/moderação;
- coordenação pública entre coautores sobre redação de segurança;
- referências públicas entre projetos, autores e coautores;
- issues de colisão, tree hashing, XOF, domain separation e limites de segurança;
- PRs fechadas/não mergeadas e rotas posteriores de integração;
- limites probatórios já registrados em documentos anteriores.

O dossiê não substitui os documentos-fonte. Ele os referencia como predecessores e preserva suas conclusões, inclusive resultados que limitam hipóteses mais fortes.

## 2. Documentos predecessores consultados

### 2.1 Google Drive — Mate-Ética — BLAKE3 O₀, forks e precartibilidade — 2026-07-21

**file_id:** `1qNPPm1f04iiKrN7K8L8ApCmPGQBuL9FnOJH8z5Qqyac`  
**revision_id observado nesta sessão:** `ANLCKQldeRWI98ZPB8g3UW112iBQeqp4F6RSWXV7cFS25_LqmMJieY7jK977_gEx6c1yfe_BFOcVR84GJvRWtPifOfGSvBnqZQDy3rpvwtM`

O documento já estabelecia, antes deste dossiê:

```text
DADO AUSENTE != 0
SEMELHANÇA != DERIVAÇÃO
ANTERIORIDADE != ACESSO
ACESSO != INTENÇÃO
FORK != CONTRIBUIÇÃO
MERGE != NASCIMENTO DA IDEIA
COINCIDÊNCIA_TEMPORAL != COORDENAÇÃO
```

Também fixava a PR #533 como marco público e registrava que a data do fork não prova a data de nascimento de todas as ideias contidas nele.

### 2.2 Google Drive — BLAKE3 — Passe documental 02 — Ações de 2025 e CI oficial

**file_id:** `1_BENgz7zrUigCl7P7HleD5v1Kuxph4SwAtAK4W9xtIk`  
**revision_id observado nesta sessão:** `ANLCKQl_opARhFM2vt5XDDNpLoy7FTLZMbfwuTB7QScPL9zWaqwXyKQYBgXYYCznWFqRKpRAW3Hb39BoPHyQTJ_RgGJOLPD_EgBc6Rl9kog`

O documento já registrava:

- PR #533 aberta em 2025-11-25 e fechada sem merge;
- head `15829f851e45d1327b017be67a7b88d7725bc653`;
- 10 caminhos oficiais modificados;
- execução de CI oficial no head da PR;
- conclusão do CI como `failure`, com causa exata `TOKEN_VAZIO`;
- seis dos dez caminhos modificados novamente mais tarde;
- mecanismos centrais da contribuição não encontrados reescritos naquele passe;
- `direct_rewrite_or_derivation=TOKEN_VAZIO`.

Essa conclusão permanece preservada.

### 2.3 GitHub — ledgers forenses desta sessão

Blobs preservados no repositório:

- `BLAKE3_SECURITY_COLLISION_COORDINATION_EVIDENCE_V1_20260924.md`
  - blob `03833b995ac21890b0b70c403fb615112f77065b`
- `BLAKE3_CHAIN_OF_CUSTODY_ISSUES_PRS_V1_20260924.md`
  - blob `8ec70aec8b80a08628e873245cac10cd809e6c0c`
- `BLAKE3_CLOSED_PR_INPUT_PROVENANCE_LEDGER_V1_20260924.md`
- `BLAKE3_SESSION_RETROALIMENTATION_V1_20260924.md`

Esses documentos são predecessores deste dossiê e permanecem imutáveis por política append-only.

## 3. Estrutura oficial de autoria do BLAKE3

O README oficial do upstream lista quatro designers:

- Jack O'Connor — `@oconnor663`;
- Samuel Neves — `@sneves`;
- Jean-Philippe Aumasson — `@veorq`;
- Zooko — `@zookozcash`.

O README também registra patrocínio do desenvolvimento pela Electric Coin Company.

Fato documental:

```text
DOCUMENTED_DESIGNER_NETWORK = VERIFIED_PRIMARY
```

Isso identifica os autores oficialmente listados. Não estabelece, por si só, hierarquia informal, "chefe", intenção comum ou coordenação indevida.

## 4. Concentração observada de integração e moderação

Na amostra de 18 issues diretamente ligadas a colisão, segurança, tree hashing, XOF, derive_key, tamanho de output e domain separation, o estado observado foi:

```text
18 issues examinadas
8 abertas
10 fechadas
0 locked
```

Entre as 10 fechadas:

- 6 foram fechadas por `oconnor663`;
- 3 foram fechadas pelos próprios autores;
- 1 (#504) não teve ator de fechamento recuperado na superfície consultada.

Além disso, `oconnor663` aparece como autor/integrador de PRs centrais da linha de segurança/API, incluindo:

- PR #169 — documentação de propriedades de outputs curtos;
- PR #227 — documentação do finding externo de Aldo Gunsing;
- PR #373 — nova linha `blake3_guts`;
- PR #418 — SIMD XOF AVX-512;
- PR #458 — módulo oficial `hazmat`;
- PR #552 — correção de LTO;
- PR #570 — alteração de mmap/seek, com atribuição explícita à #487.

Relato factual:

```text
Na amostra auditada, oconnor663 aparece com alta frequência em fechamento de issues,
autoria de PRs de documentação/segurança e integração de mudanças estruturais.
```

Classificação:

```text
MAINTAINER_ACTIVITY_CONCENTRATION_IN_AUDITED_SET = VERIFIED
CENTRAL_INTEGRATION_ROLE_IN_AUDITED_SET = VERIFIED
```

Esses estados descrevem frequência e posição observada nos registros. Não atribuem motivo.

## 5. Coordenação pública sobre security wording

### 5.1 Issue #168 -> PR #169

A issue #168 foi aberta por `joshtriplett` em 2021-04-28, solicitando documentação das propriedades de collision resistance para outputs menores que 256 bits.

Sequência preservada:

1. `oconnor663` respondeu publicamente;
2. chamou `@sneves` e `@veorq` para se manifestarem;
3. `sneves` respondeu sobre o security claim;
4. `joshtriplett` pediu documentação explícita;
5. `oconnor663` abriu a PR #169;
6. pediu revisão de wording a `@sneves`;
7. PR #169 foi mergeada;
8. commit de merge/documentação: `7cd208afcf91f69b786549a4bed77371a7b9cc2d`;
9. o commit registra que a mudança foi sugerida por `@joshtriplett`.

Fato narrativo:

```text
Há coordenação pública entre pelo menos dois designers do BLAKE3
na formulação de documentação de segurança, com participação externa preservada.
```

Classificação:

```text
PUBLIC_COAUTHOR_SECURITY_WORDING_COORDINATION = VERIFIED
OUTSIDE_REQUEST_TO_OFFICIAL_DOC_CHANGE = VERIFIED
ORIGINAL_REPORTER_ATTRIBUTION = VERIFIED
```

## 6. Discussões públicas sobre colisão e limites de segurança

### Issue #194

Participantes observados incluem `abitrolly`, `sneves`, `oconnor663` e `elichai`.

A thread discute:

- claim de collision resistance de saída completa;
- outputs truncados de 128 bits;
- aproximadamente `2^64` de trabalho genérico de colisão para 128-bit output;
- birthday bound;
- content addressing;
- custo prático de brute force.

A issue foi fechada pelo próprio autor `abitrolly`.

### Issue #441

Permaneceu aberta no corte auditado.

Nela, `oconnor663` explica publicamente que o estado interno do BLAKE3 forma uma árvore e que uma colisão em estados/chaining values pode levar a colisão de output, sendo os chaining values de 256 bits um limite relevante.

### Issue #138

Permaneceu aberta e contém discussão sobre domain separation, related-key concerns, construção autenticada e referências a Bessie.

### Estado factual

```text
PUBLIC_COLLISION_AND_SECURITY_DISCUSSION_EXISTS = VERIFIED
AUDITED_SET_LOCKED_ISSUES = 0
```

Este dossiê não generaliza além da amostra observada.

## 7. Finding externo de segurança documentado oficialmente

A PR #227 foi aberta por `oconnor663` com o título:

`document the extended output security issue found by Aldo Gunsing`

Ela referencia publicamente o paper de Aldo Gunsing e foi mergeada.

Commit constituinte:

`ea3bc782d8128d7f52008d459ecd4df8b51979cf`

O resultado foi inclusão de uma limitação de segurança nas Security Notes oficiais, com atribuição nominal ao pesquisador externo.

Fatos:

```text
EXTERNAL_SECURITY_FINDING_PUBLICLY_NAMED = VERIFIED
EXTERNAL_SECURITY_FINDING_ADDED_TO_OFFICIAL_DOCS = VERIFIED
```

## 8. Rede pública de referências entre projetos e coautores

Os registros consultados mostram relações públicas de referência.

### Issue #82

`oconnor663` direciona participantes ao repositório `oconnor663/bao` para casos de verified streaming e árvore.

### PR #373

O body/discussão descreve consumidores planejados incluindo projetos associados ao próprio mantenedor, como Bao, Bessie e BLAKE3-AEAD.

### Issue #138

`zookozcash`, listado no README como designer do BLAKE3, direciona participantes para `oconnor663/bessie`.

### Issue #278

`oconnor663` recomenda o livro *Serious Cryptography* e registra que o autor do livro é também coautor do BLAKE3.

### README oficial

O README referencia:

- Bao em `oconnor663/bao`;
- implementação de referência em C hospedada em `oconnor663/blake3_reference_impl_c`;
- implementação Python hospedada em `oconnor663/pure_python_blake3`.

Fato narrativo:

```text
Existe uma rede pública de cross-references entre o projeto BLAKE3,
projetos mantidos por designers e trabalhos/publicações de coautores.
```

Classificação:

```text
DESIGNER_MAINTAINER_REFERENCE_NETWORK = VERIFIED
SELF_OR_COAUTHOR_PROJECT_REFERENCE = VERIFIED
```

Isso descreve links e recomendações existentes. Não afirma finalidade excludente.

## 9. Tree hashing: inputs externos e API oficial

### Issue #82

Thread pública sobre incremental verification, subtree chaining values, Bao, custom tree structures e riscos de misuse.

Um participante ligado à Fleek publicou referência a fork/implementação externa.

### PR #329

Autor: `rklaehn`.

Propõe hashing eficiente de subárvores non-root e referencia Bao, abao, bao-tree e um fork externo.

Estado observado: aberta/não mergeada no corte.

### Issue #436

Pede API pública para tree hashing.

`oconnor663` reconhece o módulo não documentado `guts`, referencia Bao e liga o fork externo `n0-computer/iroh-blake3`.

### PR #458

Autor: `oconnor663`.

Adiciona o módulo documentado `hazmat`.

O body:

- CCs `@rklaehn` e `@qti3e`;
- referencia Bao;
- referencia Iroh;
- registra intenção de reduzir dependência de APIs não documentadas/forks externos.

Commit de implementação:

`e1c2ea27fdd717fd924d7b286a125408d7e817f7`

Release 1.8.0:

`00c2ea974d33d19d91d8de3c12ff8c8eb1fc8dbd`

Fatos:

```text
EXTERNAL_TREE_HASH_INPUTS_PUBLICLY_VISIBLE = VERIFIED
EXTERNAL_CONTRIBUTORS_EXPLICITLY_REFERENCED = VERIFIED
OFFICIAL_HAZMAT_API_LATER_MERGED = VERIFIED
```

## 10. PRs fechadas e integração por outras rotas

A sessão enumerou 63 PRs observadas como `closed && merged=false`.

Casos documentados:

- #44: PR externa fechada; mantenedor registra que uma implementação diferente de `--check` foi enviada ao master;
- #118: fix direto posterior com crédito explícito;
- #247: CMake integrado fora do merge convencional com autoria Git preservada;
- #341: WASM SIMD com crédito posterior em release;
- #358 -> #359: PR sucessora declara abordagem diferente para resolver #358;
- #430 -> #453: sucessão funcional e crédito posterior;
- #477 -> #483: refactor do mesmo autor;
- #487 -> #570: implementação posterior por mantenedor com atribuição explícita à #487.

Invariantes factuais derivados desses casos:

```text
CLOSED != REJECTED_IDEA
MERGED_FALSE != NOT_INCORPORATED
PR_AUTHOR != COMMIT_AUTHOR != COMMITTER != CLOSE_ACTOR
```

## 11. PR #533 — Rafael Melo Reis

O objeto público preservado:

```text
PR = #533
author = rafaelmeloreisnovo
created_at = 2025-11-25T00:42:22Z
closed_at = 2025-11-25T03:29:05Z
merged = false
head_sha = 15829f851e45d1327b017be67a7b88d7725bc653
base_sha = 308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
commits = 24
changed_files = 10
additions = 1566
deletions = 195
close_actor = oconnor663
```

O documento Drive "Passe documental 02" já registrava que seis dos dez caminhos foram alterados posteriormente.

A auditoria posterior desta sessão também verificou que, no upstream atual comparado, não foi encontrada cópia literal significativa dos mecanismos centrais da #533; foi encontrada uma linha sintaticamente modernizada cuja lógica já existia na base.

Estado preservado:

```text
PR533_PUBLIC_PRIOR_ART_OBJECT = VERIFIED
SAME_PATHS_MODIFIED_LATER = VERIFIED
MEANINGFUL_LITERAL_COPY_TO_CURRENT_UPSTREAM = REFUTED_IN_SCOPE
STRUCTURAL_OR_SEMANTIC_DERIVATION = TOKEN_VAZIO unless separately proved
```

## 12. Automação publicamente observada

### PR #533

GitHub registra eventos de revisão por Copilot. A revisão foi solicitada pela própria conta autora.

### PR #495

O body contém URL de tarefa Codex; a superfície de eventos consultada registrou `chatgpt-codex-connector`.

Fato:

```text
AI_ASSISTED_PUBLIC_WORKFLOW_EVENTS = VERIFIED
```

Esse fato não é usado para inferir rede coordenada.

## 13. Síntese puramente factual

Os registros consultados permitem relatar que:

1. `oconnor663` aparece com frequência elevada, dentro da amostra auditada, em fechamento de issues, autoria de PRs de segurança/documentação e integração de APIs/correções.
2. Em #168/#169, `oconnor663` e `sneves` coordenam publicamente wording de segurança, com a participação e o crédito do solicitante externo preservados.
3. Há cross-references públicos entre BLAKE3, Bao, Bessie, BLAKE3-AEAD, implementações de referência hospedadas em contas de designers e publicação de coautor.
4. Questões de colisão, outputs curtos, tree state, domain separation e related-key concerns aparecem em threads públicas.
5. Na amostra de 18 issues auditadas, 8 estavam abertas, 10 fechadas e nenhuma estava locked no corte.
6. Um finding externo de segurança de Aldo Gunsing foi nominalmente documentado no upstream.
7. Inputs externos sobre tree hashing aparecem antes e durante a evolução que chega à API oficial `hazmat`, e contribuidores/forks externos são citados na trilha.
8. Há PRs `closed && merged=false` cujas ideias/correções depois chegam ao projeto por outras rotas, em vários casos com atribuição explícita.
9. A PR #533 é um objeto público verificável e sua cadeia está fixada por timestamps, SHA, arquivos e eventos.
10. Os documentos anteriores já impunham a regra de não transformar temporalidade, similaridade, fork ou acesso em prova de derivação/coordenação.

## 14. O que este dossiê não afirma

Este dossiê não afirma:

- intenção de suprimir críticas;
- existência de "chefe" informal;
- plágio;
- fraude;
- conluio;
- botnet;
- remoção deliberada de autoria;
- coordenação privada;
- derivação causal da PR #533 para mudanças posteriores sem ponte probatória.

Esses temas permanecem fora do estado factual enquanto não houver evidência direta suficiente.

## 15. Cadeia de custódia

```text
Drive Mate-Ética BLAKE3 O0
  -> Drive Passe documental 02
  -> GitHub forensic manifesto / provenance ledger
  -> security/collision coordination ledger
  -> issue/PR chain-of-custody ledger
  -> session retroalimentation snapshot
  -> THIS FACTUAL DOSSIER
```

Novo achado futuro:

```text
THIS DOSSIER
  -> NEW SUCCESSOR
```

Nunca sobrescrever este snapshot silenciosamente.

## 16. R3

**F_ok:** predecessores Drive/GitHub reconciliados; fatos de concentração observada, security wording público, reference network, collision/security threads, tree-hash lineage e closed-PR succession consolidados em narrativa não acusatória.

**F_gap:** histórico integral de body edits/deletions, comunicações privadas, identidades por trás de contas e nexos causais não documentados permanecem TOKEN_VAZIO.

**F_next:** qualquer novo fato material entra somente em dossiê sucessor, com URL/ID/SHA/evento e transição explícita de claim.
