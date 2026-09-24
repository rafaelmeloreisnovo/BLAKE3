# BLAKE3 — Cadeia de Custódia de Issues, PRs e Evidências V1

**Projeto de destino:** rafaelmeloreisnovo/BLAKE3  
**Fonte primária:** BLAKE3-team/BLAKE3  
**Data do levantamento:** 2026-09-24  
**Base do fork no início desta materialização:** `90596266ae201a5b183895adbab02ad88087ff81`  
**Classe:** `FORENSIC_CHAIN_OF_CUSTODY / FACTS_ONLY / APPEND_ONLY / FAIL_CLOSED`

## 0. Regra de custódia

Este documento não atribui intenção, fraude, plágio, coordenação ilícita ou supressão. Ele preserva:

```text
SOURCE
  -> OBJECT ID / URL
  -> ACTOR
  -> TIMESTAMP / STATE
  -> COMMENT / EVENT ANCHOR
  -> PR / COMMIT / RELEASE EDGE
  -> CURRENT CLASSIFICATION
  -> GAP
```

Invariantes:

```text
SOURCE != ARTEFATO != EXECUCAO != EVIDENCIA != CLAIM
ISSUE != COMMENT != PR != COMMIT != RELEASE
CLOSED != REJECTED_IDEA
MERGED_FALSE != NOT_INCORPORATED
TEMPORAL_PROXIMITY != CAUSAL_DERIVATION
TOKEN_VAZIO != 0
```

O corpo atual de uma issue ou PR não é tratado como prova imutável de seu texto histórico original. A cadeia dá mais peso a IDs, URLs canônicas, SHAs Git, eventos, autores, committers, referências cruzadas e releases.

## 1. Conjunto de issues de colisão / segurança / árvore

Foram preservadas 18 issues diretamente relacionadas a resistência a colisão, tamanho de saída, árvore/Merkle, XOF, derivação de chave e limites de segurança:

| Issue | Autor | Estado | Criada | Fechada | Tema principal |
|---|---|---|---|---|---|
| #13 | WildCryptoFox | closed | 2020-01-10 | 2020-02-04 | derive_key context / domain separation / misuse resistance |
| #70 | FlogramMatt | closed | 2020-03-22 | 2020-04-15 | número de rounds / margem de segurança |
| #82 | 17dec | open | 2020-04-27 | — | verificação incremental / árvore BLAKE3 / Bao |
| #98 | funny-falcon | closed | 2020-07-11 | 2020-07-11 | single-block / estado interno / preimage-collision question |
| #107 | philiprbrenan | open | 2020-08-19 | — | digest size |
| #123 | gitmko0 | closed | 2020-10-13 | 2020-10-13 | outputs 224/128 bits / collision bound |
| #138 | riastradh | open | 2020-12-04 | — | deterministic authenticated cipher / domain separation / related-key concerns |
| #163 | FrankC01 | closed | 2021-03-16 | 2021-03-16 | 512-bit output / XOF |
| #168 | joshtriplett | closed | 2021-04-28 | 2021-05-18 | documentar collision resistance de saídas curtas |
| #194 | abitrolly | closed | 2021-09-01 | 2021-09-17 | collision resistance / content addressing / birthday bound |
| #208 | g0blin-NN | open | 2021-11-08 | — | b3sum -l / checksums longos/curtos |
| #246 | VictorTaelin | open | 2022-05-05 | — | alterar word size / segurança de variante |
| #274 | phantomcraft | closed | 2022-11-28 | 2022-11-28 | BLAKE3 usado como BLAKE2X / XOF |
| #278 | PavelWolfDark | closed | 2022-12-18 | 2022-12-18 | minimum secure output / collision resistance |
| #396 | elichai | open | 2024-05-28 | — | tamanho de MAC / truncation |
| #436 | josnyder-2 | open | 2024-12-03 | — | API pública de tree hashing |
| #441 | ghost | open | 2025-01-07 | — | limite de segurança em 256 bits / collision in tree state |
| #504 | ghost | closed | 2025-08-07 | 2025-08-15 | derive_key / XOF state / encrypt-then-MAC |

Estado observado no corte:

```text
issues_examined = 18
open = 8
closed = 10
locked = 0 in the audited set
```

Nos eventos de fechamento preservados nesta auditoria:
- `oconnor663` fechou #13, #70, #123, #163, #168 e #278;
- os próprios autores fecharam #98, #194 e #274;
- #504 estava fechado, mas o ator de fechamento não veio identificado na superfície de eventos recuperada: `TOKEN_VAZIO`.

## 2. Âncoras de comentários — quem introduziu qual argumento

Os IDs de comentário abaixo são preservados como âncoras públicas. Nesta passagem, o conector retornou o ID e URL canônica, mas não expôs `created_at` em todos os registros de comentário; o timestamp exato desses comentários permanece recuperável pela própria plataforma/arquivo independente e não foi inventado.

### Issue #13 — derive_key / contexto

URL: https://github.com/BLAKE3-team/BLAKE3/issues/13

Participantes observados: `WildCryptoFox`, `Luro02`, `oconnor663`, `efimio16`.

Âncoras:
- comment `573219903`, `oconnor663`: explica que `&str` foi escolha deliberada de API para desencorajar misuse e remete à especificação;
- comment `581955667`, `oconnor663`: anuncia fechamento por enquanto e possível revisão futura;
- comment `582002052`, `Luro02`: registra preocupação de discoverability porque issues fechadas não aparecem por padrão;
- comment `582040301`, `oconnor663`: mantém preferência pela API atual e pede caso real que justifique mudança;
- comments `4526546945` / `4526621372`: novo caso de uso em 2026 e resposta do mantenedor reiterando que a restrição foi deliberada.

Custódia:
```text
outside API concern
 -> extended public discussion
 -> maintainer closes
 -> later new user revives same class of use case in comments
```

### Issue #82 — árvore / verificação incremental

URL: https://github.com/BLAKE3-team/BLAKE3/issues/82

Participantes observados: `17dec`, `oconnor663`, `cesarb`, `fadedbee`, `qti3e`.

Âncoras:
- `620164772`, `oconnor663`: direciona para Bao e explica subtree chaining values;
- `621131802`, `cesarb`: propõe granularidade maior para aproveitar SIMD e reduzir metadata;
- `621269765`, `oconnor663`: concorda que tamanhos maiores podem ser usados em aplicação Bao-like;
- `621311350`, `cesarb`: propõe `finalize_subtree` / API menos propensa a misuse;
- `621328481`, `oconnor663`: discute warnings e regras de consistência de chunk/non-root finalization;
- `1501159222`, `qti3e`: apresenta fork da Fleek Network com hasher incremental e árvore completa;
- `2800042729`, `oconnor663`: informa que v1.8.0 adicionou `blake3::hazmat`, cobrindo vários casos discutidos.

Custódia:
```text
issue #82
 -> design discussion
 -> external forks / use cases
 -> later issue #436 / PR #329
 -> PR #458 hazmat
 -> release 1.8.0
```

### Issue #123 — outputs curtos

URL: https://github.com/BLAKE3-team/BLAKE3/issues/123

Participantes observados incluem `gitmko0`, `oconnor663`, `xnox`, `boogerlad`, `sneves`.

Âncoras:
- `707800498`, `oconnor663`: explica que BLAKE3 produz qualquer byte length e que reduzir output reduz garantias;
- `707804175`, `oconnor663`: explica que outputs curtos não são funções independentes como variantes SHA-2;
- `708441818`, `oconnor663`: discute birthday bound para 128-bit output;
- `767195283`, `oconnor663`: explicitamente defere detalhes de security claim a `@sneves`;
- `774172456`, `sneves`: registra limite de claim comprometido e expectativa para preimage/second-preimage.

### Issue #138 — domain separation / related-key / AEAD

URL: https://github.com/BLAKE3-team/BLAKE3/issues/138

Participantes: `riastradh`, `oconnor663`, `PaulGrandperrin`, `k0001`, `sneves`, `zookozcash`.

Âncoras:
- `738954037`, `riastradh`: condições de injectivity/domain separation;
- `739100189`, `oconnor663`: discussão de streaming, chunking, Bao e performance;
- `761170443`, `k0001`: apresenta implementação externa `baile`;
- `761297027`, `riastradh`: aponta dependência potencial de related-key resistance;
- `774175745`, `sneves`: afirma que um chosen-plaintext related-key distinguisher seria ataque válido à compression function;
- `1728292337`, `zookozcash`: referencia `oconnor663/bessie`.

Estado: thread aberta no corte.

### Issue #168 — collision resistance -> documentação oficial

URL: https://github.com/BLAKE3-team/BLAKE3/issues/168

Participantes: `joshtriplett`, `oconnor663`, `sneves`.

Âncoras:
- `829367851`, `oconnor663`: responde sobre short-output security;
- `829423453`, `sneves`: comenta ataques generic/better-than-generic;
- `829609667`, `joshtriplett`: pede documentação explícita;
- `830462118`, `oconnor663`: abre PR #169 em resposta;
- `843367481`, `oconnor663`: registra merge.

Edge:
```text
issue #168
 -> PR #169
 -> head ffd7699bcf7452d03c4cd1dc75b403bf80389bce
 -> merged commit 7cd208afcf91f69b786549a4bed77371a7b9cc2d
```

### Issue #194 — collision resistance / content addressing

URL: https://github.com/BLAKE3-team/BLAKE3/issues/194

Participantes: `abitrolly`, `sneves`, `oconnor663`, `elichai`.

Âncoras:
- `917235152`, `sneves`: full-output collision claim;
- `917617185`, `sneves`: fórmula de bound por output size;
- `917761114`, `oconnor663`: truncation/XOF equivalence para outputs curtos;
- `920982044`, `elichai`: birthday-bound/pigeonhole explanation;
- `921039741`, `oconnor663`: corrige interpretação de tamanho mínimo de arquivo;
- `921138133`, `oconnor663`: discute custo prático e limitações de throughput;
- `921548198`, `abitrolly`: encerra a discussão quanto ao próprio uso.

Fechamento: executado pelo próprio autor `abitrolly`.

### Issue #208 — b3sum -l e segurança de outputs

URL: https://github.com/BLAKE3-team/BLAKE3/issues/208

Participantes: `g0blin-NN`, `oconnor663`.

Âncoras:
- `963384710`, `oconnor663`: explica que checkfiles com tamanhos não padrão não são suportados e cita perda de segurança para hashes curtos;
- `1092173652`, `oconnor663`: diferencia XOF use cases de checksum-file convencional.

Estado: aberta.

### Issue #246 — variante de 60-bit words

URL: https://github.com/BLAKE3-team/BLAKE3/issues/246

Participantes: `VictorTaelin`, `oconnor663`, `odiferousmint`.

Âncora `1119171326`, `oconnor663`: mudar word size é tratado como desenho de nova hash function, não simples parâmetro.

Estado: aberta.

### Issue #274 — BLAKE3/BLAKE2X

URL: https://github.com/BLAKE3-team/BLAKE3/issues/274

Participantes: `phantomcraft`, `oconnor663`.

Âncora `1328520949`: o próprio autor corrige sua premissa e fecha a issue.  
Âncora `1328577592`: O'Connor responde mesmo após o self-close sobre espaço de parâmetros/XOF.

### Issue #278 — minimum secure output

URL: https://github.com/BLAKE3-team/BLAKE3/issues/278

Participantes: `PavelWolfDark`, `oconnor663`.

Âncoras:
- `1356637303`: distingue collision resistance de preimage resistance;
- `1356766163`: birthday bound para 128-bit output e recomendação de default 256-bit.

Fechada por `oconnor663`.

### Issue #396 — MAC truncation

URL: https://github.com/BLAKE3-team/BLAKE3/issues/396

Participantes observados: `elichai`, `lulcat`, `oconnor663`.

Âncora `2307614243`, `oconnor663`: 128-bit MAC é descrito como escolha razoável para muitas aplicações, distinguindo MAC security de collision resistance.

Estado: aberta.

### Issue #436 — API de tree hashing

URL: https://github.com/BLAKE3-team/BLAKE3/issues/436

Participantes: `josnyder-2`, `oconnor663`, `devinrsmith`, `nazar-pc`.

Âncoras:
- `2513404470`, `oconnor663`: reconhece `guts`, Bao e fork `iroh-blake3`;
- `2518150027`: crosslink para #82;
- `2581344233`, `devinrsmith`: oferece tree traversal API;
- `2800041185`, `oconnor663`: registra que v1.8.0 introduziu `hazmat`.

Estado: aberta.

### Issue #441 — collision inside tree state

URL: https://github.com/BLAKE3-team/BLAKE3/issues/441

Autor: `ghost`. Estado: aberta.

Âncora `2579214289`, `oconnor663`: explica que BLAKE3 usa árvore interna e que uma colisão no estado/tree chaining values limita a segurança, com chaining values de 256 bits como bound.

### Issue #504 — derive_key / XOF state

URL: https://github.com/BLAKE3-team/BLAKE3/issues/504

Autor: `ghost`. Criada 2025-08-07, fechada 2025-08-15.

Âncoras:
- `3165271427`, `oconnor663`: confirma que output >256 bits não adiciona entropia além do estado interno e discute o modelo proposto;
- `3190882574`, `ghost`: registra que a resposta resolveu as dúvidas.

Ator de fechamento: `TOKEN_VAZIO` na superfície recuperada.

## 3. Grandes PRs — custódia de implementação

| PR | Autor | Estado | Commits | Files | + / - | Função na cadeia |
|---|---|---|---:|---:|---:|---|
| #169 | oconnor663 | merged | 1 | 2 | +26/-4 | documentação short-output security após #168 |
| #227 | oconnor663 | merged | 4 | 3 | +38/-22 | documenta finding externo de Aldo Gunsing |
| #329 | rklaehn | open | 5 | 2 | +103/-7 | non-root subtree hashing / external API pressure |
| #373 | oconnor663 | merged | 5 | 6 | +1894/-0 | nova `blake3_guts` portable implementation |
| #418 | oconnor663 | merged | 14 | 13 | +2896/-30 | AVX-512 XOF; body atribui assembly a `@sneves` |
| #442 | devinrsmith | open | 1 | 1 | +128/-31 | keyed/derive_key variants for guts |
| #445 | silvanshade | merged | 2 | 18 | +848/-102 | oneTBB parallelism C |
| #457 | ibmibmibm | open | 3 | 16 | +396/-37 | OpenMP, declara mimetizar #445 |
| #458 | oconnor663 | merged | 31 | 8 | +831/-113 | `hazmat`; cita Bao/Iroh e CCs externos |
| #496 | kikairoya | merged | 1 | 2 | +2/-2 | Cygwin xof_many AVX-512 workaround |
| #533 | rafaelmeloreisnovo | closed/unmerged | 24 | 10 | +1566/-195 | contribuição pública RMR; benchmark/platform/SIMD |
| #552 | oconnor663 | merged | 2 | 2 | +35/-8 | LTO fix; `Fixes #550` |
| #577 | extremeandy | open | 1 | 3 | +134/-0 | NEON xof_many AArch64; closes #576 |
| #582 | poliebotics | merged | 2 | 2 | +66/-8 | stack buffer alignment / timing bands |
| #589 | rafaelmeloreisnovo | closed/unmerged | 547 | 362 | +36119/-547 | grande topology/build contribution RMR |

SHAs de custódia:
- #169 head `ffd7699bcf7452d03c4cd1dc75b403bf80389bce`, merge `7cd208afcf91f69b786549a4bed77371a7b9cc2d`;
- #227 head/merge `d295410aad19d70e4b7ab9b93b216a77fb8d40e4`; constituent security commit `ea3bc782d8128d7f52008d459ecd4df8b51979cf`;
- #373 head `5558fa46239742720d84c46edb0544732adf4db8`;
- #418 head `4cc34b0e85002f6fef3cd8b264343172cfbc9f58`;
- #458 head `9004cbcbde679ac5f8e24d698d2aa7e17d848d17`; implementation commit `e1c2ea27fdd717fd924d7b286a125408d7e817f7`; release 1.8.0 `00c2ea974d33d19d91d8de3c12ff8c8eb1fc8dbd`;
- #496 head `d62babb7ebb01c8ac4aaa580f4b49071a639195e`;
- #533 head `15829f851e45d1327b017be67a7b88d7725bc653`, base `308b95dfa15d5a0aa8cb3c5534ffd90d76122c46`;
- #552 head `299b1e209ef80fd2261567635198331f683c4831`;
- #577 head `17b34df8af314142afcfdad096813638397aa30a`;
- #582 head `0e4eda6805bd125beb084155ea21c48cbaa649d8`;
- #589 head `809f6213674798cbcd0e76f3fbf060ebe1d68a1d`, base `6aab490a26124663329dfd3961b8469f8fdb158b`.

## 4. PRs fechadas / não mergeadas — custódia de sucessão

### #44 -> implementação diferente no master

PR #44:
- autor: `phayes`;
- criada 2020-01-26;
- fechada 2020-05-15;
- `merged=false`;
- head `81457396bf8e298d74a3bd00aff6897bcdfad2b0`.

Upstream depois materializa `--check` por outra implementação, com commits incluindo:
- `dc2a79d2669c92795c7214761a0568d629a100e4`;
- `c6a99dbb239522b1f99950faefc2b072ee3b1a53`;
- `c5c07bb337d0af7522666d05308aaf24eef3709c`.

O fechamento registra publicamente que uma implementação diferente havia sido enviada ao master.

### #118 -> fix direto com crédito

PR #118:
- autor: `pascal-cuoq`;
- big-endian behavior;
- `merged=false`;
- head `cfa51afe3da2b38ef9d1b7d30ffc5419f864b2f7`.

Commit upstream `0b13637ae31c2e7e1a471e39258606fabb01685e` corrige big-endian e preserva crédito aos pesquisadores.

### #247 -> CMake incorporado fora de merge convencional

PR #247:
- autor: `SteveGremory`;
- criada 2022-06-05;
- fechada 2023-05-01;
- head `37f7b549944b0ba6db280d5b7ea8bfebafdaa798`;
- `merged=false`.

Commit `1569e345552187a6640b2eaff540a84b525854e2` preserva `SteveGremory` como Git author e integra CMake.

### #341 -> WASM SIMD com crédito de release

PR #341:
- autor: `monoid`;
- 19 commits, 7 files, +869/-2;
- head `7b5bff62dfe360be16372f38c3a34290c2b1475c`;
- `merged=false`.

Posteriormente, upstream recebe WASM SIMD e release 1.7.0 credita `@monoid (#341)`.

### #358 -> #359

#358:
- autor `rui314`;
- ARM32 NEON flag;
- `merged=false`;
- head `7a1bbbf9794ee90527e375191e3d9961d33565f7`.

#359:
- autor `BurningEnlightenment`;
- criada antes do fechamento de #358;
- body declara abordagem diferente para resolver #358;
- merge head `3465fe455e6cfd98d94f6d5fe1de9c4e2d566b33`.

### #430 -> #453

#430:
- autor `dbohdan`;
- `b3sum --tag`;
- `merged=false`;
- head `2bdc8236fdea537b06c523b534f2cfe4ecbb8861`.

#453:
- autor `leahneukirchen`;
- merged;
- head `c721a95806173975d469a2260f57294fcc5a3dde`.

Release posterior credita as duas linhas de contribuição.

### #477 -> #483

Mesmo autor `silvanshade`.

#477:
- `merged=false`;
- head `89e3bf919ea508dcc75e59d9bb9ea64ff507b416`.

#483:
- título declara ser refactor de #477;
- merged;
- head `b8fd52381adf74ee6fe7ab44c2caaa8fe778e01b`.

### #487 -> #570

#487:
- autor `nabijaczleweli`;
- mmap conditions;
- `merged=false`;
- head `5f872424dc01cc06aead748023327b4248a00ff0`.

#570:
- autor `oconnor663`;
- merged;
- body registra explicitamente que a ideia foi originalmente sugerida por `@nabijaczleweli` em #487;
- head `5c568a65d20f4da822dde8f89c12199bfb805c82`;
- o body também registra crédito ao Codex por uma otimização específica.

### #533 — Rafael Melo Reis

- author `rafaelmeloreisnovo`;
- created `2025-11-25T00:42:22Z`;
- closed `2025-11-25T03:29:05Z`;
- close actor observado: `oconnor663`;
- `merged=false`;
- 24 commits / 10 files / +1566/-195;
- head `15829f851e45d1327b017be67a7b88d7725bc653`.

Event history preservado no ledger anterior inclui Copilot review solicitado pelo próprio autor e fechamento posterior por `oconnor663`.

### #589 — Rafael Melo Reis

- author `rafaelmeloreisnovo`;
- created `2026-09-23T07:51:50Z`;
- closed `2026-09-23T07:59:18Z`;
- `merged=false`;
- 547 commits / 362 files / +36119/-547;
- head `809f6213674798cbcd0e76f3fbf060ebe1d68a1d`.

O evento de fechamento foi executado por `rafaelmeloreisnovo`, servindo como controle contra inferir que todo `closed/unmerged` equivale a rejeição de mantenedor.

## 5. Cadeias de custódia principais

### A. Collision documentation

```text
joshtriplett / issue #168
 -> oconnor663 response
 -> sneves security input
 -> joshtriplett asks for docs
 -> PR #169 by oconnor663
 -> commit 7cd208af...
 -> official documentation
```

### B. External security finding

```text
Aldo Gunsing / ePrint 2022/283
 -> PR #227 by oconnor663
 -> commit ea3bc782...
 -> Security Notes in official docs
```

### C. Tree hashing / external forks / official hazmat

```text
issue #82 (17dec)
 -> Bao / guts discussion
 -> qti3e Fleek fork
 -> PR #329 (rklaehn)
 -> issue #436 (josnyder-2)
 -> external Iroh / devinrsmith inputs
 -> PR #458 (oconnor663)
 -> commit e1c2ea27...
 -> release 1.8.0 00c2ea974...
```

### D. Closed PR -> successor with explicit lineage

```text
#358 rui314 -> #359 BurningEnlightenment
#477 silvanshade -> #483 silvanshade
#487 nabijaczleweli -> #570 oconnor663
```

### E. Large performance / backend lineage

```text
#418 AVX-512 XOF
 -> later xof_many platform expansion
 -> #577 NEON xof_many AArch64

#445 oneTBB
 -> #457 OpenMP explicitly mimics #445

#550 LTO issue
 -> #552 maintainer fix

zooko/bench-hashes#2
 -> #582 alignment fix
```

Esta seção preserva arestas documentadas; não declara derivação onde a própria fonte não estabelece a aresta.

## 6. Grandes PRs versus PRs fechadas

Para auditoria, duas dimensões devem ser separadas:

```text
size = commits/files/additions/deletions
state = open/closed/merged
```

Exemplos:
- #589 é enorme por escala (547 commits, 362 files) e `merged=false`;
- #533 é materialmente grande (24 commits, 10 files, +1566/-195) e `merged=false`;
- #418 é grande e mergeada (14 commits, 13 files, +2896/-30);
- #458 é grande e mergeada (31 commits, 8 files, +831/-113);
- #373 tem +1894 linhas e foi mergeada;
- #341 tem 19 commits e +869 linhas, ficou `merged=false`, mas teve crédito posterior.

Logo:

```text
PR_SCALE != MERGE_STATE != IMPACT != ATTRIBUTION
```

## 7. Limites desta cadeia de custódia

1. Comentários GitHub recuperados nesta passagem vieram com IDs/URLs, porém o conector não forneceu `created_at` em todos os comentários. Nenhum timestamp de comentário ausente foi inventado.
2. GitHub não fornece, nesta superfície, histórico completo e imutável de todas as edições de body/title.
3. Conteúdo deletado, mensagens privadas, moderação fora do repositório e identidade real por trás de contas permanecem fora do escopo.
4. Similaridade entre duas mudanças não é registrada como derivação sem diff/AST/attribution edge.
5. A cadeia de custódia preserva o que ocorreu na superfície pública; não converte ausência de evidência em prova de inexistência.

## 8. Estado probatório

```text
ISSUE/PR PUBLIC PROVENANCE               = PROVADO
CLOSE ACTOR WHERE EVENT EXISTS           = PROVADO
COMMENT ID / URL ANCHORS                 = PROVADO
PR HEAD/BASE SHA                         = PROVADO
MERGED/NON-MERGED STATE                  = PROVADO
EXPLICIT SUCCESSOR ATTRIBUTION           = PROVADO WHERE CITED
EXTERNAL INPUT -> OFFICIAL DOC/API       = PROVADO IN LISTED CHAINS
HIDDEN BODY EDIT HISTORY                 = TOKEN_VAZIO
DELETED CONTENT                          = TOKEN_VAZIO
PRIVATE COORDINATION                     = TOKEN_VAZIO
MOTIVE                                   = TOKEN_VAZIO
```

## 9. R3

**F_ok:** 18 security/collision/tree issues indexed; actors, states, event edges and comment anchors preserved; major PR scale and SHAs preserved; closed/non-merged successor chains preserved; explicit provenance edges separated from inference.

**F_gap:** immutable history of body edits/deletions, complete private communication and some missing comment timestamps remain unavailable on the current source surface.

**F_next:** any future extension must append new source objects and edges without rewriting this snapshot; if an object changes, preserve old snapshot as predecessor and add a successor record.
