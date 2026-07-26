<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Arquitetura RMR Crypto Registry

## 1. Objetivo arquitetural

A arquitetura separa quatro domínios que não podem ser confundidos:

```text
PRIMITIVA CRIPTOGRÁFICA
        ↓ adaptador explícito
OPERAÇÃO RMR
        ↓ evidência normalizada
CADEIA DE CUSTÓDIA
        ↓ catálogo e gates
REGISTRO DE REFERÊNCIAS
```

- A primitiva calcula digest, assinatura, KEM, cifra ou protocolo conforme sua própria especificação.
- O RMR executa wrappers, coleta ambiente, normaliza entradas e produz artefatos.
- A custódia registra origem, hash, commit, flags, host, tempo e resultado.
- O registro descreve projetos externos sem incorporar seus códigos.

## 2. Invariantes

| ID | Invariante | Falha se |
|---|---|---|
| CR-01 | O núcleo BLAKE3 upstream permanece fora de `rmr/crypto/` | arquivo upstream é alterado por esta trilha |
| CR-02 | Todo arquivo autoral possui cabeçalho ou exceção formal | autoria/licença não é rastreável |
| CR-03 | Nenhum código de terceiro é vendorizado silenciosamente | `vendored=true` sem evidência e licença |
| CR-04 | SHA-256 e BLAKE3 permanecem primitivas distintas | um digest é rotulado como o outro |
| CR-05 | Registro não equivale a recomendação de produção | catálogo é publicado como certificação |
| CR-06 | Relação de fork exige prova de ancestralidade | nome semelhante vira prova de parentesco |
| CR-07 | Claims possuem status e falsificador | conclusão não pode ser auditada |
| CR-08 | Validação local não requer rede | resultado depende de fonte mutável não congelada |
| CR-09 | Dados sensíveis não entram no registro | segredo, chave ou PII é versionado |
| CR-10 | Decisão jurídica final é humana | automação altera termos ou compatibilidade legal |

## 3. Modelo de componentes

### 3.1 Perfil de digest

`SHA256_PROFILE.md` descreve o backend SHA-256 atualmente exposto por `rmr/core/pai_hash.h`.

Interface observada:

```c
void pai_sha256_init(pai_sha256_ctx *ctx);
void pai_sha256_update(pai_sha256_ctx *ctx, const uint8_t *data, size_t len);
void pai_sha256_final(pai_sha256_ctx *ctx, uint8_t out[32]);
void pai_sha256_hex(const uint8_t hash[32], char out[65]);
int pai_sha256_file(const char *path, uint8_t out[32]);
```

O registro não cria uma nova função hash e não modifica constantes, rounds ou semântica SHA-256.

### 3.2 Registro

`registry/architectures.json` é um snapshot declarativo. Cada família contém:

- identificador estável;
- categoria técnica;
- repositório upstream declarado;
- papel de uso no RMR;
- política de importação;
- status de licença;
- três candidatos relacionados;
- estado de ancestralidade;
- estado de auditoria e claim.

### 3.3 Schema

`schemas/architecture-registry.schema.json` descreve o formato pretendido. O validador local aplica um subconjunto determinístico do contrato sem bibliotecas externas.

### 3.4 Ledger de claims

`claims/claims.jsonl` registra afirmações separadas de evidências. Cada linha contém:

```text
claim_id + tipo + texto + estado + evidência + falsificador + próximo passo
```

Nenhuma linha com `claim_allowed=false` pode ser apresentada como conclusão confirmada.

### 3.5 Auditor

`tools/validate_registry.py` executa:

1. parsing estrito JSON/JSONL;
2. validação de versão e metadados;
3. cardinalidade 10 × 3;
4. unicidade de IDs e repositórios;
5. proibição de vendoring implícito;
6. coerência de estados legais e de parentesco;
7. verificação dos KATs SHA-256 `empty` e `abc` pela biblioteca padrão;
8. digest SHA-256 do registro.

`tools/audit_crypto_registry.sh` é o ponto de entrada portátil.

## 4. Fluxo de custódia

```text
artefato bruto
  ├─ origem observada
  ├─ timestamp UTC
  ├─ caminho/URL
  └─ commit/ref
        ↓
normalização explicitamente versionada
        ↓
SHA-256 de custódia + digest adicional opcional
        ↓
manifesto imutável por execução
        ↓
claims ledger
        ↓
gate humano: aceitar | rejeitar | TOKEN_VAZIO
```

O digest prova igualdade de bytes dentro do escopo; não prova autoria, licitude, segurança, causalidade ou identidade de pessoa.

## 5. Estados permitidos

### 5.1 Evidência

- `VERIFIED_PRIMARY`
- `VERIFIED_LOCAL`
- `DOCUMENTED`
- `DECLARED`
- `TOKEN_VAZIO`
- `REFUTED_IN_SCOPE`

### 5.2 Licença

- `VERIFIED_AT_COMMIT`
- `DECLARED_BY_REPOSITORY`
- `TOKEN_VAZIO_NOT_REVIEWED`
- `CONFLICT_REQUIRES_HUMAN_REVIEW`

### 5.3 Relação de fork

- `VERIFIED_DIRECT_PARENT`
- `VERIFIED_ANCESTOR`
- `RELATED_DISTRIBUTION`
- `TOKEN_VAZIO_PARENT_VERIFICATION`
- `NOT_A_FORK`

## 6. Gates de promoção

| Gate | Requisito mínimo |
|---|---|
| G0 Catálogo | nome e repositório identificados |
| G1 Origem | commit/ref congelado e metadados registrados |
| G2 Licença | texto e notices lidos no commit congelado |
| G3 Integridade | hashes do material bruto e importado |
| G4 Build | compilador, flags, alvo e logs preservados |
| G5 Correção | vetores oficiais ou testes equivalentes passam |
| G6 Segurança | ameaça, uso permitido e limitações documentados |
| G7 Publicação | revisão humana e linguagem de claim aprovada |

A entrada no catálogo é apenas G0. Ela não implica G2, G5, G6 ou G7.

## 7. Compatibilidade low-level

A implementação deve favorecer o contrato já adotado no RMR:

- C determinístico;
- possibilidade de perfil `no-libc`/freestanding quando tecnicamente suportado;
- sem heap em hot path quando tamanho puder ser fixado;
- sem dependência de rede no auditor;
- sem chaves, tokens ou segredos no repositório;
- builds por alvo com flags e ABI registradas;
- ARMv7, AArch64 e x86_64 tratados como alvos distintos.

Isso é diretriz de engenharia, não afirmação de que todos os projetos catalogados atendem a esses requisitos.

## 8. Ameaças consideradas

- confusão de autoria entre upstream e RMR;
- licença ausente ou incompatível;
- fork falso ou parentesco presumido;
- troca de algoritmo por rotulagem incorreta;
- digest correto sobre entrada errada;
- manifesto mutável sem recibo;
- benchmark sem equivalência de saída;
- uso de MD5/SHA-1 como prova adversarial;
- segredo versionado;
- claim promocional acima da evidência.

## 9. Não objetivos

Este módulo não pretende:

- certificar FIPS, Common Criteria, ISO ou qualquer produto;
- criar uma nova primitiva criptográfica;
- substituir revisão jurídica;
- garantir que os trinta candidatos sejam forks diretos;
- recomendar automaticamente uma biblioteca para produção;
- importar código de terceiros.

## 10. Critério de pronto

O módulo estará `STRUCTURALLY_VERIFIED` quando o auditor e os testes passarem. Licenças, parentesco e segurança de cada projeto continuarão em estados independentes até auditoria específica.