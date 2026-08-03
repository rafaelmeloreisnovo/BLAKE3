# BLAKE3 — Mapa forense Ω7×8 das 56 metodologias

**Estado:** `METHOD_MAP`  
**Gerado em:** `2026-08-03T10:04:00-03:00`  
**Objetivo:** investigar o repositório como DAG multidimensional, não como linha cronológica simples.

## Modelo

Cada evento é representado por:

```text
E = <tempo, objeto, ref, identidade, conversa, execução, artefato, inferência>
```

Cada região aplica sete operações:

```text
direta | inversa | recursiva | comparativa | anômala | falsificável | generativa
```

## 1. Temporalidade

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| T01 | Delta autor–committer | `author_date`, `committer_date`, delta | P0 |
| T02 | Normalização UTC/offset | data original e UTC | P0 |
| T03 | Correlação com evento GitHub | push, PR, review e fechamento | P0 |
| T04 | Monotonicidade pai–filho | inversões temporais no DAG | P1 |
| T05 | Futuro/passado extremo | clocks e timestamps anômalos | P1 |
| T06 | Receipt temporal externo | log, email, archive, NTP ou arquivo | P0 |
| T07 | Antiderivada temporal | primeira janela causal possível | P1 |

## 2. Grafo, branches e refs

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| G01 | Mirror de refs | `show-ref`, refs de PR e tags | P0 |
| G02 | Reflog local | posições antigas de refs | P0 |
| G03 | Objetos unreachable/dangling | `fsck` e `lost-found` | P0 |
| G04 | Range-diff antes/depois | série reescrita ou force-push | P0 |
| G05 | Replace refs e grafts | substituições de objetos/pais | P0 |
| G06 | Branch órfã/renomeada/excluída | head original e último receipt | P0 |
| G07 | Tags leves/anotadas | tagger, target, movimento e assinatura | P0 |

## 3. Identidade e proveniência

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| A01 | Autor–committer–pusher | três papéis separados | P0 |
| A02 | Assinaturas criptográficas | status GPG/SSH/S/MIME | P0 |
| A03 | Auditoria `.mailmap` | identidade original e canônica | P1 |
| A04 | Trailers de autoria/revisão | coautoria, review e geração | P1 |
| A05 | Receipt de agente de IA | agente, task, contexto e intervenção | P0 |
| A06 | `blame --reverse -M -C` | sobrevivência, movimento e cópia | P1 |
| A07 | Commit vazio/sentinela | finalidade, workflow e contexto | P0 |

## 4. Forks, recorrência e rotas paralelas

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| F01 | DAG de forks | origem e ancestrais comuns | P0 |
| F02 | `patch-id --stable` | patch equivalente com SHA diferente | P0 |
| F03 | Fingerprint de cherry-pick | reaplicação e trailers removidos | P1 |
| F04 | Clone semântico por AST | lógica equivalente reescrita | P1 |
| F05 | Spoofing de nome de branch | fork parecendo ref oficial | P0 |
| F06 | Fork privado/excluído | região inacessível marcada `TOKEN_VAZIO` | P0 |
| F07 | Vendorização/espelhos | sobrevivência em consumidores | P1 |

## 5. Conversas, contradições e moderação

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| C01 | Histórico de edição | versões e horários do comentário | P0 |
| C02 | Comentário/review excluído | lacuna e referências residuais | P0 |
| C03 | Tratamento comparativo | tempo, tom e feedback por papel | P1 |
| C04 | Grafo de contradições | claim anterior versus ação posterior | P1 |
| C05 | Cross-links | PR, issue, commit e discussão correlatos | P1 |
| C06 | Lock/minimize/spam | alteração de visibilidade/interação | P0 |
| C07 | Conversa externa | email/chat/reunião como `TOKEN_VAZIO` | P1 |

## 6. CI, bots e automação

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| X01 | Histórico de workflows | teste removido ou reduzido | P0 |
| X02 | Run ligado ao SHA | commit exato realmente executado | P0 |
| X03 | Custódia de logs/artifacts | hashes, prazo e expiração | P0 |
| X04 | `pull_request_target` | execução privilegiada de fork | P0 |
| X05 | Pin de actions/workflows | dependência por SHA, tag ou branch | P0 |
| X06 | Identidade/permissões de bot | token, escopo e ator efetivo | P1 |
| X07 | Gate do campo `Testing` | comando, ambiente, log e exit code | P0 |

## 7. Releases e cadeia de fornecimento

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| R01 | Tag→commit→release | cadeia exata de publicação | P0 |
| R02 | Imutabilidade da release | tag e assets bloqueados ou mutáveis | P0 |
| R03 | Hash longitudinal de assets | mesmo nome, bytes e datas | P0 |
| R04 | Build reproduzível | fonte versus binário | P0 |
| R05 | SBOM/attestation | dependências e proveniência | P0 |
| R06 | Diff binário/desmontagem | código ausente do source diff | P1 |
| R07 | Reprodução antivírus | falso positivo versus payload | P1 |

## 8. Inferência reversa e falsificabilidade

| ID | Método | Saída esperada | Urgência |
|---|---|---|---|
| I01 | `range-diff` de gerações | permutação da série de commits | P0 |
| I02 | Anomalia vetorial | score de tempo/grafo/autoria/conversa | P1 |
| I03 | Bayes auditável | atualização explícita de hipóteses | P2 |
| I04 | Sequência recorrente | introdução→falha→correção→crédito | P1 |
| I05 | Evidência negativa calibrada | ausência esperada com limite | P1 |
| I06 | Dependência reversa | consumidores e impacto por versão | P1 |
| I07 | Red-team de claims | tentativa documentada de refutação | P0 |

## Vetor de prioridade

```text
P0 = preservar ou reproduzir antes que desapareça
P1 = correlacionar e reduzir ambiguidade
P2 = inferir somente depois da cadeia P0/P1
```

## Regra causal

Anomalia não prova intenção. Uma alegação de dolo exige, no mínimo:

```text
AUTORIA + CONHECIMENTO_PRÉVIO + AÇÃO + OCULTAÇÃO + BENEFÍCIO + REPETIÇÃO + CONTRAEVIDÊNCIA_TESTADA
```

Na falta de qualquer elo material, registrar `TOKEN_VAZIO`.
