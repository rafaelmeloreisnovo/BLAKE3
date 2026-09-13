<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Protocolo de cadeia de custódia

## 1. Unidade de evidência

Uma unidade de evidência RMR deve ser identificável sem depender da memória do operador.

```yaml
evidence_id: RMR-CRYPTO-<UTC>-<sequence>
source_type: REPOSITORY | FILE | ARCHIVE | LOG | EXECUTION | DECLARATION
source_locator: <repo/path/url/local-reference>
source_commit: <sha> | TOKEN_VAZIO
observed_at_utc: <RFC3339 Z>
observed_at_unix_s: <integer>
observed_at_local: <RFC3339 numeric offset>
utc_offset: <+HH:MM|-HH:MM>
timezone_id: <IANA name> | TOKEN_VAZIO
clock_source: SYSTEM_CLOCK | NTP | GPS | TSA | MANUAL | TOKEN_VAZIO
clock_sync_status: SYNCHRONIZED | UNVERIFIED | TOKEN_VAZIO
operator: <identity>
raw_sha256: <hex> | TOKEN_VAZIO
time_attestation_digest_sha256: <hex> | TOKEN_VAZIO
additional_digest: <algorithm:value> | NONE
custody_status: CAPTURED | VERIFIED | TOKEN_VAZIO
```

## 2. Regras

- preservar o bruto antes de normalizar;
- nunca recalcular e sobrescrever o digest anterior sem registrar nova versão;
- usar SHA completo para commits;
- distinguir data observada de data alegada;
- distinguir arquivo original de cópia exportada;
- registrar ferramenta e versão;
- manter resultado negativo e falha de teste;
- não incluir segredo no manifesto;
- não usar timestamps isolados como prova conclusiva de autoria;
- usar UTC RFC3339 com `Z` + Unix epoch seconds como instante canônico;
- preservar o horário civil local apenas com offset numérico explícito;
- tratar horário de verão como política local de offset, nunca como alteração do UTC;
- exigir equivalência entre UTC, epoch e representação local antes de selar o registro;
- distinguir selo/digest temporal local, assinatura digital e timestamp externo confiável.

## 3. Snapshot de repositório

Para auditoria de licença ou fork:

```text
owner/repo
commit SHA completo
árvore de arquivos relevante
LICENSE/NOTICE/COPYING naquele commit
metadados parent/source/fork quando disponíveis
hash do arquivo de licença
hash do patch ou diff analisado
```

Branches, tags e páginas mutáveis podem ser referências de navegação, mas a evidência deve ser fixada em commit ou artefato imutável.

## 4. Snapshot de execução

Registrar:

- comando exato;
- diretório de trabalho;
- variáveis relevantes;
- sistema operacional e arquitetura;
- compilador/interpreter e versão;
- flags de compilação/link;
- entrada e tamanho;
- stdout/stderr;
- exit code;
- artefatos gerados;
- hashes dos artefatos;
- status do relógio;
- commit executado.

## 5. Canonicalização

A canonicalização deve ser versionada. Nunca afirmar que dois objetos semanticamente iguais possuem os mesmos bytes sem contrato explícito.

```text
raw bytes → canonicalizer(version) → canonical bytes → digest
```

O manifesto deve preservar digest do bruto e do canônico quando ambos existirem.

### 5.1 Canonicalização temporal

O contrato normativo está em `rmr/crypto/FORENSIC_TIME_PROFILE.md`.

```text
UTC RFC3339 Z + Unix epoch
        |
        +--> instant_digest
        |
hora local + offset + timezone + clock status
        |
        +--> local_attestation_digest
```

O `instant_digest` não depende de timezone ou horário de verão. O
`local_attestation_digest` preserva essas informações como contexto forense.

O core `rmr/freestanding_custody16` continua sem relógio. A camada externa
vincula o registro temporal ao digest do selo/evidência.

```text
local_attestation_digest != assinatura digital
assinatura local != timestamp externo confiável
```

O utilitário executável é `rmr/crypto/tools/forensic_time_attest.py`.

## 6. Hashchain

Uma cadeia simples pode ser construída por:

```text
record_hash[n] = SHA256(domain || version || prev_hash || canonical_record[n])
```

Mas esse encadeamento, sozinho, não impede um agente com acesso total de reconstruir toda a cadeia. Para proteção contra reescrita, utilizar assinatura, timestamp confiável ou armazenamento externo imutável, em trilha futura e separada.

## 7. Promoção de claims

Cada claim deve apontar para evidência específica:

```yaml
claim_id: <id>
status: TOKEN_VAZIO | DOCUMENTED | VERIFIED_PRIMARY | VERIFIED_LOCAL | REFUTED_IN_SCOPE
evidence_ids: []
falsifier: <condição observável>
reviewer: <humano> | TOKEN_VAZIO
```

Sem `evidence_ids`, o estado máximo é `DECLARED`.

## 8. Retenção

- não apagar evidência contraditória;
- usar superseding record em vez de edição silenciosa;
- manter schema/versionamento;
- registrar migrações;
- armazenar backups conforme classificação e privacidade;
- evitar duplicar dados pessoais sem finalidade.

## 9. Saída mínima do auditor

```text
registry_path
registry_sha256
schema_version
family_count
candidate_count
sha256_kat_status
claims_status
exit_code
```

## 10. Limites

Cadeia de custódia técnica aumenta rastreabilidade. Ela não produz, isoladamente, presunção jurídica universal, certificação, autoria ou validade probatória automática.