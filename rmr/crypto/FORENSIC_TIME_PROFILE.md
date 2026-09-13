<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Forensic Time Profile V1 — UTC canônico + atestação local

## 1. Objetivo

Formalizar o tempo como parte da cadeia de custódia sem transformar horário civil em fonte de verdade criptográfica.

O perfil separa três camadas:

```text
INSTANTE CANÔNICO
  = UTC RFC3339 com Z + Unix epoch seconds

CONTEXTO CIVIL LOCAL
  = timestamp local com offset numérico + identificador de zona quando conhecido

ÂNCORA/ASSINATURA
  = digest do registro canônico + assinatura destacada ou timestamp externo, quando existirem
```

Invariantes:

```text
UTC != horário local
UTC não adota horário de verão
horário de verão altera o offset/regra civil local, não o instante UTC
hash != assinatura
atestado local != timestamp externo confiável
SOURCE != ARTEFATO != EXECUÇÃO != EVIDÊNCIA != CLAIM
TOKEN_VAZIO != 0
```

## 2. Representação canônica do instante

O perfil V1 usa simultaneamente:

```yaml
created_at_utc: YYYY-MM-DDTHH:MM:SSZ
created_at_unix_s: <integer>
```

Os dois campos DEVEM representar exatamente o mesmo segundo.

`created_at_utc` segue o perfil de timestamps da Internet do RFC 3339 com offset zero expresso por `Z`.

O Unix epoch é uma representação numérica auxiliar para ordenação, comparação e reconstrução independente de convenções de exibição.

V1 trabalha em precisão de segundos. Frações de segundo e representação explícita de leap second ficam para versão posterior.

## 3. Contexto civil local

A observação local deve preservar:

```yaml
local_time_rfc3339: YYYY-MM-DDTHH:MM:SS-03:00
utc_offset: -03:00
timezone_id: America/Sao_Paulo | TOKEN_VAZIO
```

O offset numérico é obrigatório. O `timezone_id` é contexto forense e não participa da conversão canônica: regras IANA/políticas civis podem mudar ao longo do tempo.

Regra:

```text
parse(local_time_rfc3339).to_utc() == created_at_utc
```

Logo, duas representações locais diferentes podem apontar para o mesmo instante:

```text
12:00:00-03:00 == 13:00:00-02:00 == 15:00:00Z
```

Se uma jurisdição adota ou remove horário de verão, muda o contexto local; o instante UTC permanece o mesmo.

## 4. Estado do relógio

O registro deve declarar a origem e o grau de confiança do relógio:

```yaml
clock_source: SYSTEM_CLOCK | NTP | GPS | TSA | MANUAL | TOKEN_VAZIO
clock_sync_status: SYNCHRONIZED | UNVERIFIED | TOKEN_VAZIO
clock_uncertainty_ms: <integer >= 0> | omitido
```

`SYNCHRONIZED` somente deve ser usado quando existir evidência própria da sincronização. Ler o relógio do sistema não prova, sozinho, sincronização com UTC.

## 5. Vínculo com a evidência

O tempo não deve flutuar separado do objeto custodiado.

Registro mínimo:

```yaml
schema: rmr.forensic_time.record.v1
evidence_id: <stable id>
evidence_digest:
  algorithm: SHA-256 | BLAKE3-256 | GIT-OID-SHA1
  value: <hex>
source_commit: <40 hex> | TOKEN_VAZIO
created_at_utc: <RFC3339 Z>
created_at_unix_s: <integer>
local_time_rfc3339: <RFC3339 numeric offset>
utc_offset: <numeric offset>
timezone_id: <IANA name> | TOKEN_VAZIO
clock_source: <declared source>
clock_sync_status: <declared status>
operator: <identity> | TOKEN_VAZIO
claim_allowed: false
```

## 6. Dois digests, duas perguntas

### 6.1 Instant digest

Responde:

> qual evidência foi associada a qual instante universal?

```text
instant_digest =
SHA256(
  "RMR-FORENSIC-INSTANT-V1\0" ||
  canonical_json({
    schema,
    evidence_id,
    evidence_digest,
    created_at_utc,
    created_at_unix_s
  })
)
```

O horário local, timezone e regra de horário de verão NÃO entram nesse digest.

### 6.2 Local attestation digest

Responde:

> como o mesmo instante foi observado/declarado localmente e em qual contexto de relógio?

```text
local_attestation_digest =
SHA256(
  "RMR-FORENSIC-LOCAL-ATTESTATION-V1\0" ||
  canonical_json({
    instant_digest_sha256,
    local_time_rfc3339,
    utc_offset,
    timezone_id,
    clock_source,
    clock_sync_status,
    clock_uncertainty_ms,
    source_commit,
    operator
  })
)
```

Assim:

```text
mesmo instante + outro offset civil
=> instant_digest igual
=> local_attestation_digest diferente
```

Esse comportamento é intencional.

## 7. Preimage para assinatura local

Para uma assinatura destacada futura:

```text
signature_preimage_sha256 =
SHA256(
  "RMR-FORENSIC-TIME-SIGNATURE-V1\0" ||
  bytes(local_attestation_digest_sha256)
)
```

A assinatura criptográfica deve cobrir esse preimage (ou o `local_attestation_digest` diretamente, conforme o esquema versionado).

Campos futuros/externos:

```yaml
signature_scheme: <Ed25519 | SSH | OpenPGP | ...> | TOKEN_VAZIO
signature_key_id: <fingerprint> | TOKEN_VAZIO
signature_value: <detached signature> | TOKEN_VAZIO
signature_verification: VERIFIED | PRESENT_UNVERIFIED | TOKEN_VAZIO
verifier_receipt: <digest/path> | TOKEN_VAZIO
```

Fronteira obrigatória:

```text
local_attestation_digest != assinatura digital
assinatura local verificável != autoridade temporal externa
```

Uma assinatura prova vínculo com uma chave; não prova, sozinha, que o relógio estava correto.

## 8. Âncora temporal externa

RFC 3161/TSA, release imutável, DOI ou outro terceiro confiável podem reforçar a anterioridade temporal.

No estado atual:

```yaml
local_time_profile: IMPLEMENTED
utc_local_equivalence_validator: IMPLEMENTED
local_attestation_digest: IMPLEMENTED
digital_signature: TOKEN_VAZIO
trusted_external_timestamp: TOKEN_VAZIO
claim_allowed: false
```

O RFC 3161 permanece uma camada diferente: uma TSA cria um token para indicar que um dado existia em determinado instante e deve usar uma fonte de tempo confiável.

## 9. Integração com Freestanding Custody16

O core `rmr/freestanding_custody16` permanece sem relógio. Isso é deliberado.

Fluxo:

```text
bytes/caminhos
-> Custody16 determinístico
-> seal/digest da evidência
-> adapter/camada de auditoria captura tempo
-> RMR-FORENSIC-TIME-V1
-> instant_digest
-> local_attestation_digest
-> assinatura local opcional
-> âncora externa opcional
```

Não injetar timezone, DST ou relógio no cálculo determinístico interno do snapshot. O registro temporal é um sidecar vinculado criptograficamente ao digest do artefato.

## 10. Falsificadores

O registro deve ser rejeitado quando:

- UTC e Unix epoch não representarem o mesmo segundo;
- timestamp local convertido para UTC divergir de `created_at_utc`;
- offset declarado divergir do offset presente no timestamp local;
- faltar offset no horário local;
- digest tiver tamanho/formato incompatível com o algoritmo declarado;
- `claim_allowed` for promovido automaticamente;
- `SYNCHRONIZED` for usado sem evidência própria;
- um digest local for chamado de assinatura digital;
- uma assinatura local for chamada de timestamp externo confiável.

## 11. Referências normativas

- RFC 3339 — Date and Time on the Internet: Timestamps.
- RFC 3161 — Internet X.509 PKI Time-Stamp Protocol.

**SOURCE != ARTEFATO != EXECUÇÃO != EVIDÊNCIA != CLAIM**
