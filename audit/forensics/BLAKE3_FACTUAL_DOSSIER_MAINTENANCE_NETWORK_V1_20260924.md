# BLAKE3 — Dossiê Factual de Integração, Moderação, Security Wording e Rede de Referências — V1

**snapshot_id:** `BLAKE3_FACTUAL_DOSSIER_MAINTENANCE_NETWORK_V1_20260924`  
**successor_of:** `BLAKE3_SESSION_RETRO_V1_20260924`  
**parent_master_at_creation:** `b8675d419ed2463b27cdadebcd0efd13ce597e1b`  
**data:** 2026-09-24  
**classe:** `FACTUAL_DOSSIER / NON_ACCUSATORY / APPEND_ONLY / FAIL_CLOSED`

## 0. Finalidade

Este dossiê descreve fatos observáveis no histórico público do projeto BLAKE3 e em documentos forenses já preservados no acervo RMR/RAFAELIA.

Ele não atribui:
- intenção;
- culpa;
- fraude;
- plágio;
- conspiração;
- supressão deliberada;
- coordenação privada;
- motivação para preservar status pessoal.

A unidade narrativa é:

```text
pessoa/conta
 -> issue/PR
 -> comentário/evento
 -> commit/release
 -> relação documental observável
```

## 1. Fontes anteriores revisadas

### GitHub — rafaelmeloreisnovo/BLAKE3

1. `audit/forensics/BLAKE3_FORENSIC_MANIFESTO_NONNEGOTIABLE_V1_20260924.md`
   - blob: `47094b308ef013ee5b1a1e8ead899e768141e518`

2. `audit/forensics/BLAKE3_CLOSED_PR_INPUT_PROVENANCE_LEDGER_V1_20260924.md`
   - blob: `73dce124234ba7ddaf93e4f1bf24316850ec9268`

3. `audit/forensics/BLAKE3_SECURITY_COLLISION_COORDINATION_EVIDENCE_V1_20260924.md`
   - blob: `03833b995ac21890b0b70c403fb615112f77065b`

4. `audit/forensics/BLAKE3_CHAIN_OF_CUSTODY_ISSUES_PRS_V1_20260924.md`
   - blob: `8ec70aec8b80a08628e873245cac10cd809e6c0c`

5. `audit/forensics/BLAKE3_SESSION_RETROALIMENTATION_V1_20260924.md`
   - blob: `69f17f64c3f13d8c52275eaabedf4d4fe5a191a5`

### Google Drive

6. `BLAKE3 — Passe documental 02 — Ações de 2025 e CI oficial`
   - file_id: `1_BENgz7zrUigCl7P7HleD5v1Kuxph4SwAtAK4W9xtIk`
   - revision observed: `ANLCKQl_opARhFM2vt5XDDNpLoy7FTLZMbfwuTB7QScPL9zWaqwXyKQYBgXYYCznWFqRKpRAW3Hb39BoPHyQTJ_RgGJOLPD_EgBc6Rl9kog`

7. `Mate-Ética — BLAKE3 O₀, forks e precartibilidade — 2026-07-21`
   - file_id: `1qNPPm1f04iiKrN7K8L8ApCmPGQBuL9FnOJH8z5Qqyac`
   - revision observed: `ANLCKQldeRWI98ZPB8g3UW112iBQeqp4F6RSWXV7cFS25_LqmMJieY7jK977_gEx6c1yfe_BFOcVR84GJvRWtPifOfGSvBnqZQDy3rpvwtM`

8. `BLAKE3 — Export Forensic Custody — Gmail Headers × NOVOexport Hash Ledger — V1 — 2026-09-24`
   - file_id: `18OD2GF2DktAH4XA26EnRWlsOJ0Um0RHy--eK2-qT2Uw`
   - revision observed: `ANLCKQkOJjA6jP9zfQpofhgGeJ4ao8kqM_ca4eiSotFS2k35STJj_B1XI8h2BKPioNhcS0ZmxhcuSFNAyAtnIBipCil8cG_08DnS9bEKCJU`

Os documentos Drive fornecem contexto de cronologia, método de inferência e custódia. Os fatos sobre atores, issues, PRs, commits e releases abaixo são ancorados nas superfícies GitHub já registradas nos ledgers anteriores.

---

# 2. Os atores documentados do desenho do BLAKE3

O README oficial consultado durante a auditoria lista quatro designers:

- `oconnor663` — Jack O'Connor;
- `sneves` — Samuel Neves;
- `veorq` — Jean-Philippe Aumasson;
- `zookozcash` — Zooko.

Esse dado estabelece um conjunto público de coautores/designers. Não estabelece hierarquia formal entre eles além das funções observáveis no repositório.

No histórico auditado, `oconnor663` aparece repetidamente como:
- autor de PRs;
- integrador de mudanças;
- participante de issues;
- ator de fechamento em issues e PRs;
- interlocutor de usuários;
- autor de documentação;
- responsável por releases/implementações em cadeias examinadas.

Isso descreve atividade pública no repositório; não é inferência sobre autoridade jurídica, influência fora do GitHub ou motivo.

---

# 3. Concentração observada de integração e moderação

No conjunto específico de **18 issues** de collision/security/tree/XOF/derive_key/output-size preservadas no snapshot anterior:

```text
issues_examined = 18
open = 8
closed = 10
locked = 0
```

Entre as 10 fechadas:
- 6 foram fechadas por `oconnor663`: #13, #70, #123, #163, #168 e #278;
- 3 foram fechadas pelos próprios autores: #98, #194 e #274;
- #504 estava fechada, mas o ator de fechamento não foi recuperado naquela superfície: `TOKEN_VAZIO`.

Além dos fechamentos, o ledger de grandes PRs registra `oconnor663` como autor de várias mudanças materialmente relevantes, entre elas:

- PR #169 — documentação de propriedades de outputs curtos;
- PR #227 — documentação do finding de segurança de Aldo Gunsing;
- PR #373 — implementação portátil do novo `blake3_guts`;
- PR #418 — aceleração SIMD inicial de XOF AVX-512;
- PR #458 — módulo oficial `hazmat`;
- PR #552 — correção de builds com LTO;
- PR #570 — mudança de mmap/seek, com atribuição a sugestão anterior de outro contribuidor.

A descrição factual permitida é:

```text
oconnor663 aparece com alta frequência em integração,
fechamento/moderação, resposta técnica e autoria de mudanças
no subconjunto auditado.
```

O tamanho e seleção da amostra não permitem transformar essa frequência em afirmação sobre intenção.

---

# 4. Coordenação pública sobre security wording: issue #168 → PR #169

A cadeia mais direta encontrada é pública e documentada.

## 4.1 Issue #168

URL:
https://github.com/BLAKE3-team/BLAKE3/issues/168

Autor:
`joshtriplett`

Tema:
documentação das propriedades de collision resistance de outputs menores que 256 bits.

A cadeia registrada nos comentários é:

1. `oconnor663` responde às perguntas de segurança;
2. `oconnor663` chama `@sneves` e `@veorq` para contribuir;
3. `sneves` responde sobre o security claim;
4. `joshtriplett` pede que a explicação seja documentada;
5. `oconnor663` abre a PR #169;
6. na PR #169, `oconnor663` pede revisão de wording a `@sneves`;
7. a PR é mergeada;
8. a issue #168 é fechada por `oconnor663`.

Comentários-âncora preservados:
- `829367851` — resposta de `oconnor663`;
- `829423453` — resposta de `sneves`;
- `829609667` — pedido de documentação de `joshtriplett`;
- `830462118` — `oconnor663` anuncia PR #169;
- `843367481` — registro de merge.

## 4.2 PR #169

Head:
`ffd7699bcf7452d03c4cd1dc75b403bf80389bce`

Merge commit:
`7cd208afcf91f69b786549a4bed77371a7b9cc2d`

O commit registra que a mudança foi sugerida por `@joshtriplett` em #168.

A documentação resultante registra, em substância, que outputs de N bits até 256 bits têm objetivo de N/2 bits de collision resistance.

O fato documentado é:

```text
pergunta externa
 -> resposta de mantenedor
 -> consulta a coautores
 -> revisão pública do security wording
 -> documentação oficial mergeada
```

Isso é coordenação pública observável de conteúdo de segurança.

---

# 5. Outros exemplos de interação entre coautores em temas de segurança

## 5.1 Issue #123

Tema: outputs de 224/128 bits e bounds de segurança.

Participantes registrados incluem:
`gitmko0`, `oconnor663`, `xnox`, `boogerlad`, `sneves`.

Comentários-âncora:
- `707800498`, `oconnor663`: explica outputs de tamanho arbitrário e redução de garantias;
- `708441818`, `oconnor663`: discute birthday bound;
- `767195283`, `oconnor663`: remete detalhes do security claim a `@sneves`;
- `774172456`, `sneves`: responde sobre limites do claim.

A sequência mostra divisão pública de interlocução técnica: O'Connor responde inicialmente e solicita/cede parte da formulação de segurança a Neves.

## 5.2 Issue #138

Tema: deterministic authenticated cipher, domain separation e related-key concerns.

Participantes registrados:
`riastradh`, `oconnor663`, `PaulGrandperrin`, `k0001`, `sneves`, `zookozcash`.

Âncoras:
- `739100189`, `oconnor663`: streaming, chunking, Bao e performance;
- `761297027`, `riastradh`: preocupação de related-key resistance;
- `774175745`, `sneves`: resposta sobre chosen-plaintext related-key distinguisher;
- `1728292337`, `zookozcash`: referência a `oconnor663/bessie`.

A thread permanecia aberta no snapshot auditado.

---

# 6. Rede pública de referências entre projetos e coautores

Os registros mostram referências cruzadas concretas.

## 6.1 Bao

Na issue #82, `oconnor663` direciona usuários a:
`oconnor663/bao`

A mesma thread discute:
- subtree chaining values;
- incremental verification;
- APIs internas;
- forks externos;
- granularidade de árvore.

## 6.2 Bessie

Na issue #138, `zookozcash`, listado como um dos designers do BLAKE3, direciona participantes a:
`oconnor663/bessie`.

## 6.3 Bao, Bessie e BLAKE3-AEAD

Na PR #373, `oconnor663` registra esses projetos como consumidores planejados da nova linha de API:
- Bao;
- Bessie;
- BLAKE3-AEAD.

## 6.4 Serious Cryptography

Na issue #278, `oconnor663` recomenda o livro *Serious Cryptography* e registra que seu autor é coautor do BLAKE3.

A narrativa factual é:

```text
projetos mantidos por um designer
<-> referências em issues/PRs do projeto
<-> referências feitas por outro designer
<-> publicação de coautor recomendada em resposta técnica
```

Essas arestas demonstram uma rede pública de referências profissionais/técnicas. Não demonstram finalidade de excluir terceiros.

---

# 7. A rede também inclui fontes e contribuidores externos

O mesmo histórico registra referências explícitas a pessoas e projetos externos.

## 7.1 Aldo Gunsing — PR #227

Título:
`document the extended output security issue found by Aldo Gunsing`

PR aberta por:
`oconnor663`

Commit constituinte:
`ea3bc782d8128d7f52008d459ecd4df8b51979cf`

A documentação oficial passou a citar o finding externo e o paper ePrint 2022/283.

## 7.2 Fleek Network — issue #82

O comentário `1501159222`, de `qti3e`, apresenta um fork da Fleek Network com hasher incremental e árvore completa.

A thread permanece no histórico público.

## 7.3 Iroh / tree hashing — #436 e #458

Na issue #436, O'Connor:
- reconhece a API `guts`;
- cita Bao;
- cita o fork externo `n0-computer/iroh-blake3`.

Na PR #458:
- `oconnor663` CCs `@rklaehn` e `@qti3e`;
- menciona explicitamente Iroh;
- descreve como objetivo reduzir a necessidade de projetos externos manterem forks próprios.

Implementação:
`e1c2ea27fdd717fd924d7b286a125408d7e817f7`

Release 1.8.0:
`00c2ea974d33d19d91d8de3c12ff8c8eb1fc8dbd`

Portanto, a rede documental observada não é composta apenas de referências entre coautores; ela também contém atribuições e referências externas.

---

# 8. Integração de contribuições por caminhos diferentes de merge convencional

Os ledgers anteriores documentam que o estado `merged=false` não descreve sozinho a história de uma contribuição.

## #44 — b3sum --check

Autor:
`phayes`

Estado:
closed / merged=false.

Durante a discussão foram tratados detalhes de filenames, escaping, Unicode, Windows paths, tamanho de hash e segurança.

Commits posteriores do upstream:
- `dc2a79d2669c92795c7214761a0568d629a100e4`;
- `c6a99dbb239522b1f99950faefc2b072ee3b1a53`;
- `c5c07bb337d0af7522666d05308aaf24eef3709c`.

No fechamento, O'Connor informou publicamente que uma implementação diferente de `--check` havia sido enviada ao master.

## #118 — big-endian

Autor:
`pascal-cuoq`.

PR não mergeada.

O commit:
`0b13637ae31c2e7e1a471e39258606fabb01685e`

faz a correção e preserva crédito aos pesquisadores relacionados ao reporte.

## #247 — CMake

Autor:
`SteveGremory`.

PR fechada sem merge.

O commit:
`1569e345552187a6640b2eaff540a84b525854e2`

preserva `SteveGremory` como Git author e integra CMake por outra rota.

## #341 — WASM SIMD

Autor:
`monoid`.

PR fechada sem merge.

A implementação aparece posteriormente e a release 1.7.0 credita `@monoid (#341)`.

## #487 → #570

#487:
`nabijaczleweli`, mmap conditions, merged=false.

#570:
`oconnor663`, mergeada.

O body da #570 registra explicitamente que a ideia foi originalmente sugerida por `@nabijaczleweli` em #487.

Esses casos mostram que O'Connor aparece em várias posições de integração: fechamento, reimplementação, commit, merge ou atribuição. Cada caso preserva seu próprio contexto e não deve ser generalizado além dele.

---

# 9. O objeto RMR: PR #533

A PR upstream #533 é um objeto de proveniência separado.

```text
author=rafaelmeloreisnovo
created_at=2025-11-25T00:42:22Z
closed_at=2025-11-25T03:29:05Z
merged=false
head=15829f851e45d1327b017be67a7b88d7725bc653
base=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
commits=24
changed_files=10
additions=1566
deletions=195
close_actor=oconnor663
```

O documento Drive `BLAKE3 — Passe documental 02` registra ainda:

```text
workflow=tests
run_id=19656415853
run_number=1549
status=completed
conclusion=failure
logs=HTTP_410_GONE
exact_failure_line=TOKEN_VAZIO
root_cause=TOKEN_VAZIO
```

O mesmo documento registra que seis dos dez arquivos da #533 foram alterados posteriormente no upstream, mas que naquele passe os mecanismos centrais específicos procurados não foram encontrados no master então examinado.

O snapshot forense posterior também realizou comparação literal e não encontrou cópia literal significativa da #533 no upstream atual; uma única linha sintaticamente modernizada tinha equivalente semântico preexistente na base.

Portanto, para #533:

```text
public prior-art object = verified
close actor = verified
official CI execution = verified
later same-path modification = verified for 6/10 in prior pass
literal meaningful copy to later upstream = refuted in scoped comparison
semantic/structural derivation = TOKEN_VAZIO
motive = TOKEN_VAZIO
```

---

# 10. Fatos que limitam uma narrativa mais ampla

O mesmo conjunto de fontes registra:

1. issue #194 teve discussão extensa sobre collision resistance e foi fechada pelo próprio autor `abitrolly`;
2. issue #441 permaneceu aberta e contém explicação pública sobre colisões no estado/tree chaining values;
3. issue #138 permaneceu aberta com discussão de domain separation e related-key concerns;
4. issues #82 e #436 permaneceram abertas discutindo estrutura em árvore e APIs internas;
5. issue #168 gerou documentação oficial de collision resistance;
6. PR #227 incorporou e nomeou um finding externo de segurança;
7. no conjunto das 18 issues auditadas, nenhuma estava locked.

Esses fatos integram o mesmo dossiê porque são parte da fotografia documental e devem permanecer junto dos fatos de concentração de atividade.

---

# 11. Narrativa factual consolidada

O histórico público examinado mostra que Jack O'Connor (`oconnor663`) exerce uma função recorrente de manutenção e integração no projeto BLAKE3. No subconjunto de segurança/colisão/árvore auditado, sua conta aparece como interlocutora frequente, como ator de seis dos dez fechamentos identificados e como autora de PRs que transformaram discussões em documentação ou APIs oficiais.

Em questões de segurança, há casos de coordenação pública entre coautores. Na issue #168, O'Connor respondeu a uma pergunta externa, convocou Samuel Neves e Jean-Philippe Aumasson para contribuir, Neves respondeu sobre o security claim, e a discussão levou à PR #169, aberta por O'Connor e submetida a revisão de wording por Neves antes de ser mergeada. Em outras threads, O'Connor remeteu questões de security claim a Neves, e Neves respondeu publicamente.

Também existe uma rede documental de referências entre projetos e publicações ligados aos designers. O'Connor referencia Bao; Zooko referencia Bessie; O'Connor registra Bao, Bessie e BLAKE3-AEAD como consumidores de APIs em desenvolvimento; e recomenda uma publicação de coautor em resposta técnica. Essas relações estão registradas nos próprios issues e PRs.

Ao mesmo tempo, o histórico contém referências explícitas a pesquisadores e contribuidores externos. Um finding de Aldo Gunsing foi incorporado às notas oficiais de segurança; forks e usos da Fleek e Iroh aparecem nas discussões de tree hashing; e a PR #458 chama contribuidores externos pelo nome antes da estabilização do módulo `hazmat`.

O histórico de PRs fechadas mostra ainda que contribuições podem seguir rotas diferentes de um merge convencional: reimplementação por mantenedor, commit direto, PR sucessora ou release com crédito. Exemplos incluem #44, #118, #247, #341 e #487→#570.

A PR #533 de Rafael Melo Reis permanece um objeto público verificável, fechado por O'Connor sem merge. Ela acionou CI oficial e tocou dez arquivos. Auditorias posteriores preservadas no Drive e no GitHub não encontraram, no escopo examinado, prova de cópia literal significativa da contribuição para o upstream posterior. Relações semânticas ou causais mais fortes permanecem não determinadas.

Assim, os fatos deste dossiê descrevem:
- atividade de integração/moderação concentrada em uma conta dentro da amostra auditada;
- coordenação pública entre coautores em formulações de segurança;
- referências cruzadas entre projetos/publicações dos designers;
- referências e créditos também dirigidos a fontes externas;
- múltiplas rotas públicas pelas quais contribuições entram ou influenciam o projeto.

O dossiê não converte esses fatos em conclusão sobre finalidade, intenção ou coordenação privada.

---

# 12. Lacunas preservadas

```text
private_coordination = TOKEN_VAZIO
motive = TOKEN_VAZIO
deleted_content = TOKEN_VAZIO
complete_immutable_body_edit_history = TOKEN_VAZIO
identity_behind_accounts = TOKEN_VAZIO
causal_derivation_PR533_to_later_changes = TOKEN_VAZIO except where independently demonstrated
```

## 13. Regra de sucessão

Este dossiê é um novo snapshot. Ele não substitui os documentos anteriores.

Qualquer fato novo deverá entrar em:

`BLAKE3_FACTUAL_DOSSIER_SUCCESSOR_<N>_<DATE>.md`

com:
- `successor_of`;
- nova fonte;
- novo SHA/event/comment;
- delta factual;
- eventual correção;
- lacunas preservadas.

Nenhuma fotografia anterior deverá ser silenciosamente reescrita.
