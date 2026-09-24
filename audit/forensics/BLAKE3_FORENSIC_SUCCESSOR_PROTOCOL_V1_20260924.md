# BLAKE3 — Protocolo de Sucessão Forense Append-Only V1

**Data:** 2026-09-24  
**Repositório:** rafaelmeloreisnovo/BLAKE3  
**Classe:** `FORENSIC_SUCCESSOR_PROTOCOL / APPEND_ONLY / FAIL_CLOSED`

## Regra central

Nenhum novo achado deve sobrescrever, reescrever silenciosamente ou substituir a fotografia forense que o precede.

```text
FORENSIC_SNAPSHOT_n
  --new evidence-->
FORENSIC_SNAPSHOT_n+1

FORENSIC_SNAPSHOT_n remains addressable by its Git blob/commit SHA.
```

Cada sucessor deve declarar:

```text
snapshot_id
successor_of
source_objects
source_urls
source_shas
observed_at
material_delta
claim_delta
evidence_class
gaps
receipt
```

## Invariantes

```text
SOURCE != ARTEFATO != EXECUCAO != EVIDENCIA != CLAIM
TOKEN_VAZIO != 0
TEMPORAL_PROXIMITY != CAUSAL_DERIVATION
CLOSED != REJECTED_IDEA
MERGED_FALSE != NOT_INCORPORATED
NEW_FINDING != REWRITE_OF_OLD_SNAPSHOT
```

## Política de mutação

### Imutáveis por política forense

- manifestos/snapshots com versão e data;
- receipts;
- índices tabulares associados a um snapshot;
- claim ledgers fechados;
- blobs referenciados por SHA.

Se um erro material for descoberto em um snapshot anterior, não corrigir o snapshot silenciosamente. Criar um novo documento:

```text
CORRECTION_SUCCESSOR_V<n>
successor_of=<snapshot anterior>
correction_of=<claim/object>
reason=<evidence>
```

### Mutáveis apenas como roteadores

Podem evoluir:
- `audit/forensics/README.md`;
- START HERE;
- índices de navegação.

Mesmo nesses roteadores, a alteração deve apontar para o novo sucessor sem apagar o predecessor.

## Estados permitidos para claims

```text
PROVADO
VERIFIED_PRIMARY
VERIFIED_TREE_IDENTITY
VERIFIED_LOCAL_MEASUREMENT
OBSERVED_CI
DECLARED_BY_AUTHOR
SUPPORTED_HYPOTHESIS
REFUTED_IN_SCOPE
TOKEN_VAZIO
```

Um sucessor pode:
- adicionar evidência;
- restringir um claim;
- refutar um claim;
- promover um claim quando a prova necessária existir.

Um sucessor não pode:
- apagar evidência desfavorável;
- converter ausência em zero;
- alterar retrospectivamente autoria, datas ou SHAs sem correction successor.

## Forma mínima de receipt

```text
snapshot_id=
successor_of=
repository=
base_master=
head=
artifact_paths=
artifact_blobs=
source_scope=
claim_gate=
gaps=
```

## Regra de cadeia

```text
PREDECESSOR --hash/URL--> SUCCESSOR
SUCCESSOR --parent pointer--> PREDECESSOR
```

A cadeia deve ser verificável nos dois sentidos por Git history e pelos campos de custódia.

## Regra epistêmica

Contra fatos não se apaga o passado: uma evidência nova muda o estado por delta explícito.

```text
old_claim_state + new_evidence -> successor_claim_state
```

Nunca:

```text
old_claim_state -> silently edited old snapshot
```
