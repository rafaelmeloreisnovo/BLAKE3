<!--
FACTUAL DOSSIER MIRROR
Drive document_id: 1CUNl4DVBFgC42T7d3BnPpkSjomDPoEOzGr6H9lB9BDQ
Drive revision: ANLCKQl7dbCxwsyTj-EdmU7wGsjWoSDeJ8OobUkjsMfX-OFEuYrRYIiWL4t9D2n7A_CEZmFBLeYMhEkBF37CYFk4J0p1yt4bbESZy76teJ8
successor_of: master@b8675d419ed2463b27cdadebcd0efd13ce597e1b
Policy: APPEND_ONLY / NON_ACCUSATORY / FAIL_CLOSED
-->

BLAKE3 — DOSSIÊ FACTUAL DE MANUTENÇÃO, SEGURANÇA E REFERÊNCIAS — V1 — 2026-09-24

STATE

FACTUAL_NARRATIVE / NON_ACCUSATORY / APPEND_ONLY / FAIL_CLOSED

REGRA

SOURCE != ARTEFATO != EXECUÇÃO != EVIDÊNCIA != CLAIM

TOKEN_VAZIO != 0

NOVO ACHADO != REESCRITA SILENCIOSA DE SNAPSHOT ANTERIOR

OBJETIVO

Reunir, em uma narrativa única e verificável, fatos já preservados nos documentos anteriores sobre:

1) participação recorrente de oconnor663 em respostas, fechamentos e integrações no subconjunto auditado;

2) interação pública entre coautores sobre redação e claims de segurança;

3) referências cruzadas entre projetos e publicações de autores/coautores;

4) rotas pelas quais issues, PRs, commits e releases se relacionam;

5) limites do que os registros públicos permitem concluir.

Este dossiê não atribui fraude, plágio, conspiração, discriminação, supressão deliberada, botnet, apropriação ilícita, hierarquia informal ou motivo estratégico. Quando um nexo ou motivo não está demonstrado, permanece TOKEN_VAZIO.

1. BASE DOCUMENTAL JÁ EXISTENTE

Este dossiê não substitui os documentos anteriores. Ele os usa como predecessores e índice de fatos já preservados, entre eles:

- BLAKE3_SECURITY_COLLISION_COORDINATION_EVIDENCE_V1_20260924.md

- BLAKE3_CLOSED_PR_INPUT_PROVENANCE_LEDGER_V1_20260924.md

- BLAKE3_CHAIN_OF_CUSTODY_ISSUES_PRS_V1_20260924.md

- BLAKE3_AUTHORSHIP_CHRONOLOGY_2025_2026.md

- BLAKE3_AI_PROVENANCE_AND_GOVERNANCE.md

- BLAKE3_DOCUMENTARY_TIMELINE_PASS_01_20260726.md

- BLAKE3_DOCUMENTARY_TIMELINE_PASS_02_2025_ACTIONS_AND_CI.md

- BLAKE3_PR533_SUCCESSOR_LEDGER_V1_20260924.json

- Drive: BLAKE3 — Passe documental 02 — Ações de 2025 e CI oficial

- Drive: Mate-Ética — BLAKE3 O₀, forks e precartibilidade — 2026-07-21

- START HERE LITE — RAFAELIA CONTINUIDADE Ω V1

A metodologia já preservada exige separar fatos públicos, medições, declarações do autor, hipóteses e lacunas.

2. AUTORES/DESIGNERS LISTADOS PELO PROJETO

O README upstream auditado lista quatro designers de BLAKE3:

- @oconnor663 — Jack O'Connor

- @sneves — Samuel Neves

- @veorq — Jean-Philippe Aumasson

- @zookozcash — Zooko

O mesmo README registra patrocínio do desenvolvimento pela Electric Coin Company.

Fato preservado: há um conjunto público e nominal de designers/coautores.

Não é inferido deste fato: hierarquia informal, “chefe”, controle coordenado, intenção comum ou finalidade de exclusão.

3. CONCENTRAÇÃO OBSERVADA DE ATOS DE RESPOSTA, FECHAMENTO E INTEGRAÇÃO

O conjunto auditado de issues diretamente relacionadas a colisão, segurança, árvore, XOF, derive_key e tamanho de output contém 18 issues:

#13, #70, #82, #98, #107, #123, #138, #163, #168, #194, #208, #246, #274, #278, #396, #436, #441, #504.

No corte preservado:

issues examinadas = 18

abertas = 8

fechadas = 10

locked = 0

Das dez fechadas:

- seis têm evento de fechamento por oconnor663: #13, #70, #123, #163, #168, #278;

- três foram fechadas pelos próprios autores: #98, #194, #274;

- #504 estava fechada, mas o ator de fechamento não veio identificado na superfície recuperada.

Assim, dentro deste subconjunto específico, oconnor663 aparece de forma recorrente em três tipos de ato observável:

- resposta técnica em issues;

- fechamento de issues;

- autoria/integração de PRs que materializam documentação ou API.

Além dos fechamentos acima, ele é autor de PRs oficiais relevantes nas cadeias auditadas, incluindo:

- #169 — documentação sobre propriedades de outputs curtos;

- #227 — documentação de finding externo de segurança;

- #373 — implementação portátil de blake3_guts;

- #418 — aceleração inicial AVX-512 do XOF;

- #458 — módulo hazmat;

- #495 — experimento AVX-512 xof_many no Windows;

- #552 — correção relacionada a LTO;

- #570 — mudança de mmap/seek com crédito explícito a uma PR externa anterior.

Formulação factual:

no subconjunto auditado, há participação recorrente de oconnor663 em resposta, fechamento e integração.

Não é inferido:

que essa concentração tenha finalidade de excluir contribuidores, controlar discurso, preservar status ou ocultar críticas.

4. SECURITY WORDING — CADEIA #168 → #169

Issue #168:

“Please document collision-resistance properties of BLAKE3 with less than 256 bits”

Autor: joshtriplett

Criada: 2021-04-28

A sequência pública preservada é:

1) oconnor663 respondeu às perguntas sobre segurança de outputs curtos;

2) oconnor663 chamou @sneves e @veorq para contribuir;

3) sneves respondeu sobre o claim de segurança;

4) joshtriplett pediu que os detalhes fossem documentados;

5) oconnor663 abriu a PR #169 em resposta ao pedido;

6) na PR #169, oconnor663 registrou que a mudança foi sugerida por @joshtriplett e pediu revisão da redação a @sneves;

7) a PR foi mergeada;

8) o commit preservado é 7cd208afcf91f69b786549a4bed77371a7b9cc2d;

9) a issue #168 foi fechada depois.

A documentação mergeada passou a explicitar, em substância, que outputs menores oferecem menos segurança e que, até 256 bits, um output de N bits visa N/2 bits de resistência a colisão.

Fatos:

- houve interação pública entre coautores sobre a redação do claim;

- a solicitação externa gerou mudança oficial de documentação;

- o autor do pedido ficou explicitamente referenciado.

5. SECURITY WORDING — OUTRAS INTERAÇÕES COM SNEVES E COAUTORES

Issue #123:

o thread tratou de outputs de 224 e 128 bits.

Nos comentários preservados:

- oconnor663 explicou que outputs curtos não constituem variantes independentes como SHA-224/SHA-256;

- discutiu o birthday bound para outputs de 128 bits;

- ao tratar de detalhes do claim, declarou que preferia deferir a @sneves;

- sneves respondeu sobre o limite do claim e expectativa de preimage/second-preimage.

Fato:

o registro público mostra O'Connor recorrendo a Neves em detalhes de formulação de segurança.

Issue #138:

o thread trata de construção autenticada determinística, domain separation e related-key concerns.

Participantes preservados incluem riastradh, oconnor663, PaulGrandperrin, k0001, sneves e zookozcash.

No thread:

- usuários externos apresentaram construções e preocupações;

- sneves comentou que um chosen-plaintext related-key distinguisher seria um ataque válido à compression function;

- zookozcash referenciou o projeto bessie de oconnor663;

- o thread permaneceu aberto no corte auditado.

6. FINDING EXTERNO DE SEGURANÇA — PR #227

PR #227:

“document the extended output security issue found by Aldo Gunsing”

Autor: oconnor663

Criada: 2022-03-02

Merge: 2022-03-03

O corpo liga diretamente ao trabalho de Aldo Gunsing, ePrint 2022/283, e pergunta a @sneves se o resumo está adequado.

Um commit constituinte preservado é:

ea3bc782d8128d7f52008d459ecd4df8b51979cf

“document the extended output security issue found by Aldo Gunsing”

A documentação oficial passou a incluir Security Notes sobre o output offset do XOF e a citar o trabalho externo.

Fatos:

- um finding externo foi nominalmente identificado;

- foi transformado em documentação oficial;

- O'Connor pediu revisão pública de wording a Neves;

- a PR foi mergeada.

7. DISCUSSÃO PÚBLICA DE COLISÕES

Issue #194:

“BLAKE3 collision resistance”

Autor: abitrolly

Criada: 2021-09-01

Participantes preservados:

abitrolly, sneves, oconnor663, elichai.

O thread discute:

- claim de 2^128 para full output;

- outputs de 128 bits e aproximadamente 2^64 de trabalho genérico de colisão;

- birthday bound;

- content addressing;

- custo prático de brute force.

A issue teve 14 comentários e foi fechada pelo próprio autor.

Issue #441:

“Why is the security limited to 256-bit?”

Criada: 2025-01-07

Estado no corte: open

O'Connor explica publicamente que BLAKE3 usa uma árvore interna e que uma colisão em estado/chaining value pode levar a colisões de output; chaining values de 256 bits limitam a segurança.

Fatos:

- a limitação foi discutida publicamente;

- a issue estava aberta no corte auditado;

- a issue não estava locked.

8. REDE REAL DE REFERÊNCIAS ENTRE PROJETOS E COAUTORES

Os registros preservados contêm as seguintes arestas públicas:

Issue #82:

oconnor663 direciona usuários para o projeto oconnor663/bao ao explicar subtree chaining values e verificação incremental.

PR #373:

oconnor663 descreve consumidores planejados ligados a Bao, Bessie e BLAKE3-AEAD.

Issue #138:

zookozcash, listado no README como designer de BLAKE3, pede aos participantes que observem o projeto oconnor663/bessie.

Issue #278:

oconnor663 recomenda o livro Serious Cryptography e registra que seu autor é coautor de BLAKE3.

PR #458:

oconnor663 cita Bao e Iroh, faz CC explícito a contribuidores externos e liga um fork externo, n0-computer/iroh-blake3.

Fato:

existem referências cruzadas públicas entre designers/coautores, seus projetos, projetos externos e publicação de coautor.

Não é inferido:

que essas referências constituam cartel, favorecimento ilícito, exclusão coordenada ou estratégia deliberada de reputação.

9. TREE HASHING, FORKS EXTERNOS E API OFICIAL

Issue #82, aberta em 2020 e ainda aberta no corte auditado, discute:

- intermediate tree nodes;

- incremental verification;

- subtree chaining values;

- Bao;

- chunk-size tradeoffs;

- riscos de misuse;

- código/fork externo apresentado por contribuidores.

Em comentário preservado, qti3e apresentou um fork ligado à Fleek Network.

PR #329:

Autor: rklaehn

Criada: 2023-07-29

Estado no corte: open / not merged

A PR propõe hashing eficiente de non-root subtrees e referencia Bao, abao, bao-tree e fork externo.

Issue #436:

Autor: josnyder-2

Criada: 2024-12-03

Estado no corte: open

O thread pede API pública de tree hashing. O'Connor reconhece a API guts não documentada, liga o fork n0-computer/iroh-blake3 e depois registra que a versão 1.8.0 adicionou blake3::hazmat.

PR #458:

Autor: oconnor663

Criada: 2025-03-27

Merge: 2025-03-31

Head: 9004cbcbde679ac5f8e24d698d2aa7e17d848d17

O corpo:

- faz CC a @rklaehn e @qti3e;

- cita Bao e Iroh;

- liga n0-computer/iroh-blake3;

- registra como objetivo reduzir dependência de APIs não documentadas e forks próprios.

Commit de implementação:

e1c2ea27fdd717fd924d7b286a125408d7e817f7

Release 1.8.0:

00c2ea974d33d19d91d8de3c12ff8c8eb1fc8dbd

Fato:

há uma cadeia pública de issues, forks externos, propostas externas, discussão com mantenedor e posterior API oficial, com referências explícitas a atores/projetos externos.

10. PRs FECHADAS E ROTAS ALTERNATIVAS DE INCORPORAÇÃO

O ledger anterior enumerou 63 PRs observadas como closed && merged=false.

O conjunto inclui exemplos factuais em que merged=false não encerra a história da contribuição:

#44:

PR externa de b3sum --check; posteriormente o mantenedor registra que uma implementação diferente foi enviada ao master.

#118:

PR externa big-endian; um commit direto posterior de oconnor663 corrige o problema e preserva crédito aos pesquisadores.

#247:

PR de SteveGremory sobre CMake; commits posteriores preservam SteveGremory como Git author e integram CMake.

#341:

PR de monoid para WASM SIMD; closed/unmerged; release posterior credita @monoid (#341).

#358 → #359:

#359 declara explicitamente uma abordagem diferente para resolver o problema de #358.

#430 → #453:

duas linhas de implementação de --tag; release posterior preserva créditos.

#477 → #483:

mesmo autor; #483 declara ser refactor de #477.

#487 → #570:

#570, de oconnor663, registra explicitamente que a ideia foi originalmente sugerida por @nabijaczleweli em #487.

Fato:

o estado merged=false de uma PR não é suficiente para determinar se uma ideia, requisito, correção ou contribuição reapareceu depois por outra rota.

11. PR #533 — FATO DOCUMENTAL E LIMITES

PR #533:

Título: A little bit of helping

Autor: rafaelmeloreisnovo

Criada: 2025-11-25T00:42:22Z

Fechada: 2025-11-25T03:29:05Z

merged=false

head=15829f851e45d1327b017be67a7b88d7725bc653

base=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46

commits=24

changed_files=10

additions=1566

deletions=195

O event log preservado registra:

- requests de review do Copilot pela própria conta autora;

- eventos de trabalho do Copilot;

- fechamento posterior por oconnor663.

O passe documental de 2025 preserva ainda que o head executou no CI oficial:

workflow=tests

run_id=19656415853

run_number=1549

status=completed

conclusion=failure

Os logs detalhados daquela execução não estavam mais disponíveis no endpoint consultado:

exact_failure_line=TOKEN_VAZIO

root_cause=TOKEN_VAZIO

A inspeção posterior encontrou alterações em 6 dos 10 paths tocados pela PR, porém não encontrou no master atual os mecanismos centrais de warm-up, RandomInput::warmup e test::black_box do benchmark exatamente como propostos.

O successor ledger preserva um caso de correspondência literal no mesmo locus:

u64::max_value() → u64::MAX

em src/lib.rs, mas a lógica equivalente já existia na base.

Fato:

PR #533 é anterioridade pública verificável para seu conteúdo.

Não está estabelecido:

que mudanças posteriores derivem causalmente da PR, salvo onde existir aresta documental específica.

12. AUTOMAÇÃO/IA — FATOS DE PROVENIÊNCIA

Documento predecessor BLAKE3_AI_PROVENANCE_AND_GOVERNANCE.md preserva:

- PR #495: body/branch associados a Codex; fechada sem merge após falha no Windows;

- PR #521: corpo declara alterações produzidas por Claude e reviews automáticos do Codex; mergeada;

- commit 6eebbbd67935cc6b8fe24da164ae968a2e3973cb: mensagem atribui a Claude a descoberta de divergência em cv_stack;

- PR #533: revisão automática do Copilot.

Fato:

o projeto possui exemplos públicos de assistência por IA em trabalhos de mantenedor e em revisão de PR externa.

Não é inferido:

rede coordenada de bots, autoria exclusiva da IA, ou finalidade adversarial.

13. O QUE OS FATOS SUSTENTAM

Sustentado pelos registros preservados:

- existe participação recorrente de oconnor663 em respostas, fechamentos e integrações dentro do subconjunto auditado;

- existem interações públicas entre coautores sobre redação e limites de claims de segurança;

- existem referências cruzadas entre projetos de mantenedor/coautores, publicação de coautor e projetos externos;

- issues externas podem gerar documentação oficial;

- findings externos podem ser nominalmente creditados e incorporados;

- PRs closed/non-merged podem ter sucessores, reimplementações ou integração por outras rotas;

- estados de PR, autoria Git, autoria cognitiva e integração são dimensões distintas.

14. O QUE NÃO É PROMOVIDO COMO FATO

Permanece TOKEN_VAZIO ou não demonstrado:

- intenção de manter determinada pessoa “no topo”;

- coordenação para silenciar discussão de colisões;

- plágio deliberado;

- apropriação ilícita;

- discriminação intencional;

- botnet;

- exclusão coordenada;

- conteúdo privado não recuperado;

- histórico completo de bodies editados ou deletados.

15. NARRATIVA FACTUAL RESUMIDA

O registro público auditado mostra um projeto com um conjunto nominal de quatro designers e uma participação recorrente de Jack O'Connor em respostas técnicas, fechamento de parte das issues e autoria/integração de mudanças upstream. Em questões de segurança, há episódios em que O'Connor chama Samuel Neves e outros coautores para revisar ou complementar a formulação de claims. A issue #168 resultou diretamente na PR #169 e em documentação oficial sobre outputs curtos; a PR #227 documentou nominalmente um finding externo de Aldo Gunsing; threads como #194 e #441 mantiveram discussão pública sobre colisões e limites de segurança.

O mesmo registro contém referências cruzadas entre Bao, Bessie, BLAKE3-AEAD, Iroh, forks externos e publicação de coautor. Em tree hashing, contribuições e necessidades externas aparecem em threads e PRs anteriores ao módulo hazmat oficial, e os participantes externos são citados publicamente na cadeia. Em paralelo, o histórico de PRs fechadas mostra que merged=false não define sozinho o destino de uma contribuição: há casos de sucessor explícito, reimplementação direta, integração fora do merge convencional e crédito posterior.

Esses fatos descrevem uma estrutura de manutenção, revisão técnica, referência e integração. Não demonstram, por si mesmos, finalidade de exclusão, supressão, apropriação ou preservação de status.

16. CADEIA DE CUSTÓDIA DESTE DOSSIÊ

Este dossiê é sucessor documental. Ele não altera os predecessores.

Predecessores Git principais:

- security/collision coordination blob: 03833b995ac21890b0b70c403fb615112f77065b

- closed-PR provenance blob: 73dce124234ba7ddaf93e4f1bf24316850ec9268

- chain-of-custody manifest blob: 8ec70aec8b80a08628e873245cac10cd809e6c0c

- authorship chronology blob: 881af0942a8b822af2caa876c18543fb78bdd08b

- AI provenance/governance blob: 5410d93d87f6fea86df49be2b469bd644352d289

- documentary timeline pass 01 blob: 62582799c35e3f597abec2d8725b4bc5df6d454e

- documentary timeline pass 02 blob: b06df699e44dd7355b8a688440d53490d5dd0082

- PR533 successor ledger blob: 06c503b81c5870080b794cbb77cee50a5b79cb1f

Drive predecessor:

- BLAKE3 — Passe documental 02 — Ações de 2025 e CI oficial

  document_id=1_BENgz7zrUigCl7P7HleD5v1Kuxph4SwAtAK4W9xtIk

  revision observada=ANLCKQl_opARhFM2vt5XDDNpLoy7FTLZMbfwuTB7QScPL9zWaqwXyKQYBgXYYCznWFqRKpRAW3Hb39BoPHyQTJ_RgGJOLPD_EgBc6Rl9kog

Este dossiê:

document_id=1CUNl4DVBFgC42T7d3BnPpkSjomDPoEOzGr6H9lB9BDQ

Regra futura:

qualquer novo fato material entra em SUCCESSOR_V2 ou CORRECTION_SUCCESSOR, apontando para este documento e preservando esta fotografia.
