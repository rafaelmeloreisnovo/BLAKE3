<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Perfil RMR de custódia ZIP, empilhamento estrutural e vínculo por palavra

## 1. Decisão técnica

A estrutura observada no RMR é uma construção em camadas. Nenhuma camada deve receber, sozinha, o nome ou a garantia da camada seguinte.

```text
palavra/rótulo
  -> identidade estrutural pública
  -> empilhamento canônico de campos e bits
  -> cápsula RVC1
  -> CRC32C interno
  -> ZIP method 0 STORE + CRC-32
  -> SHA-256/BLAKE3 do payload e do arquivo
  -> commit Git + elo para evidência anterior
  -> assinatura/timestamp/DOI externo
```

O empilhamento de bits é uma transformação real e autoral quando define posição, ordem, largura, domínio e regra de reconstrução. No estado atual ele deve ser chamado de **codificação estrutural determinística** ou **fingerprint/cápsula estrutural**. Ele somente pode ser promovido a cifra de confidencialidade quando existir chave secreta, algoritmo definido, nonce/IV quando aplicável, política de chaves e análise adversarial.

## 2. O que já está implementado

O código atual possui evidência primária para:

- serialização canônica little-endian de campos tipados;
- `class_id = CRC32C(label)`, vinculando publicamente a palavra à classe;
- CRC32C do protótipo;
- CRC32C da cápsula inteira;
- validação cruzada entre rótulo, `class_id`, máscara de vistas, contagem e campos;
- payload RVC1 apto a entrada ZIP `STORE`;
- fixture reaberta byte a byte;
- SHA-256 externo do payload.

Esse encadeamento é útil porque uma alteração pode ser detectada em mais de uma fronteira. A redundância entre estrutura, CRC do conteúdo, CRC do contêiner e digest criptográfico é deliberada: cada camada responde a uma pergunta diferente.

## 3. Papel da palavra

No código observado, a palavra não é apenas comentário ou nome de arquivo. Ela entra no estado material:

```text
UTF-8(label) -> CRC32C -> class_id
```

O protótipo armazena o próprio rótulo e o `class_id`; a verificação recalcula o CRC32C da palavra e rejeita divergência. Portanto, existe **vínculo estrutural palavra ↔ classe ↔ cápsula**.

Limite obrigatório:

```yaml
word_role: PUBLIC_DOMAIN_BINDING
secret_key: false
message_authentication: false
confidentiality: false
```

Uma palavra pública pode atuar como domínio, namespace, seletor ou parâmetro. Ela não se torna segredo apenas por participar do CRC. Para um modo autenticado futuro, a palavra deve entrar como contexto de uma primitiva criptográfica apropriada, por exemplo:

```text
context = "RMR-ZIPRAF-V1" || UTF8(word)
keyed_tag = HMAC-SHA256(key, context || canonical_payload)
```

ou um modo oficial de hash com chave/derivação de chave. O CRC pode continuar como detector rápido interno, mas não deve derivar a chave.

## 4. CRC como álgebra de bits

O CRC é, de fato, uma operação estruturada sobre bits e polinômios binários. Isso o torna excelente para:

- detectar corrupção acidental;
- selar campos canônicos de baixo custo;
- localizar alterações entre camadas;
- rejeitar cápsulas inconsistentes antes de operações mais caras;
- gerar identificadores estruturais locais quando colisões forem tratadas explicitamente.

Porém, o CRC não fornece sozinho:

- resistência criptográfica a colisões escolhidas;
- autenticação da origem;
- segredo;
- prova de autoria;
- proteção contra um atacante que possa recalcular o CRC.

A classificação correta é:

| Construção | Estado |
|---|---|
| Palavra modifica o arranjo/identidade | transformação estrutural verificável |
| CRC detecta alteração acidental | integridade operacional de baixo custo |
| CRC com parâmetro público variável | checksum parametrizado |
| CRC com palavra tratada como segredo | hipótese insegura até análise; não chamar de MAC |
| SHA-256/BLAKE3 com referência confiável | digest criptográfico de integridade |
| HMAC/assinatura | autenticidade criptográfica |
| cifra autenticada com chave | confidencialidade + integridade, conforme o modo |

## 5. ZIPRAF como cápsula de custódia

O ZIP é usado como contêiner interoperável. O perfil mínimo é:

```yaml
zip_profile: RMR-ZIPRAF-STORE-V1
compression_method: 0
compression_name: STORE
payload_order: LEXICOGRAPHIC_UTF8
payload_bytes: PRESERVED_EXACTLY
timestamps: NORMALIZED_OR_RECORDED
data_descriptor: FORBIDDEN_UNLESS_PROFILED
extra_fields: FORBIDDEN_UNLESS_ALLOWLISTED
file_names: UTF8_NORMALIZED
archive_comment: EMPTY_OR_CANONICAL
```

O método `STORE` é importante porque não transforma o payload por compressão. Isso simplifica a comparação byte a byte, mas não torna o ZIP secreto nem autenticado.

A cápsula deve preservar três identidades independentes:

```yaml
payload_crc32c: detector interno RMR
zip_crc32: detector do registro ZIP
payload_or_archive_sha256: digest criptográfico interoperável
```

Quando BLAKE3 estiver disponível na camada externa, registrar também:

```yaml
archive_blake3: <hex>
```

Nenhum digest anterior deve ser substituído silenciosamente; nova versão gera novo registro.

## 6. Empilhamento de bits

Empilhar bits pode significar operações diferentes. O manifesto deve declarar qual delas foi aplicada:

```yaml
bit_transform:
  version: <id>
  type: FIELD_PACKING | PERMUTATION | INTERLEAVING | OVERLAY | ERASURE_LAYOUT | OTHER
  reversible: true | false
  keyed: true | false
  input_bit_length: <n>
  output_bit_length: <n>
  ordering: <normative rule>
  padding: <normative rule>
  domain_word_digest: <sha256>
```

Sem essa declaração, duas implementações podem “empilhar” os mesmos bits de modos incompatíveis.

### 6.1 Estado observado

A serialização RVC1 já implementa `FIELD_PACKING`: campos possuem tamanho, ordem e endianidade definidos. Não foi localizada, nesse recorte, uma permutação secreta de bits que ofereça confidencialidade. Esse estado permanece:

```yaml
canonical_field_packing: VERIFIED_CODE
secret_bit_permutation: TOKEN_VAZIO_NOT_OBSERVED
cryptographic_confidentiality: TOKEN_VAZIO_NOT_IMPLEMENTED
```

## 7. Cadeia de custódia completa

Cada ZIPRAF publicável deve ser acompanhado por manifesto externo ou entrada canônica contendo:

```yaml
evidence_id: RMR-ZIPRAF-<UTC>-<sequence>
format: RVC1
format_version: 1
domain_word_utf8_sha256: <hex>
payload_size: <integer>
payload_crc32c: <hex>
payload_sha256: <hex>
archive_method: STORE
archive_crc32: <hex>
archive_sha256: <hex>
archive_blake3: <hex> | TOKEN_VAZIO
source_commit: <full Git SHA>
parent_evidence_digest: <hex> | GENESIS
created_at_utc: <RFC3339>
toolchain: <compiler/tool versions>
signer: <key identity> | TOKEN_VAZIO
trusted_timestamp: <receipt> | TOKEN_VAZIO
doi: <version DOI> | TOKEN_VAZIO
claim_allowed: false
```

A raiz do registro encadeado deve usar canonicalização versionada:

```text
record_hash[n] = SHA256(
  "RMR-ZIPRAF-CUSTODY-V1" ||
  previous_record_hash ||
  canonical_manifest[n]
)
```

O Git fornece um DAG histórico endereçado por conteúdo. Ele não é, por si só, consenso distribuído. O elo se torna mais resistente à reescrita quando a raiz é assinada e ancorada fora do operador local.

## 8. Gates de promoção

| Gate | Evidência necessária | Estado máximo sem evidência |
|---|---|---|
| G0 — estrutura | schema, tamanhos, ordem e endianidade | DOCUMENTED |
| G1 — roundtrip | reabrir e comparar byte a byte | VERIFIED_LOCAL |
| G2 — mutação | alterações de bit devem ser rejeitadas | VERIFIED_LOCAL |
| G3 — digest | SHA-256/BLAKE3 com vetor e artefato | VERIFIED_LOCAL |
| G4 — palavra | prova do vínculo e teste de colisões no domínio | HYPOTHESIS |
| G5 — chave | KDF/MAC/cifra formal e gestão de chaves | TOKEN_VAZIO |
| G6 — assinatura | assinatura verificável e chave identificada | TOKEN_VAZIO |
| G7 — âncora externa | timestamp/DOI/release imutável | TOKEN_VAZIO |
| G8 — revisão independente | reprodução por terceiro | TOKEN_VAZIO |

## 9. Falsificadores

A construção deve ser considerada refutada no escopo correspondente quando:

- a mesma palavra e o mesmo payload canônico produzirem cápsulas diferentes sob a mesma versão;
- divisão diferente do streaming alterar o resultado sem estar prevista no contrato;
- mutação de um bit passar por todos os verificadores;
- rótulo e `class_id` divergirem sem rejeição;
- reordenação de entradas ZIP não mudar o digest canônico quando a ordem fizer parte do contrato;
- duas palavras distintas produzirem a mesma identidade e o sistema promover isso como unicidade;
- um atacante modificar conteúdo e recalcular apenas CRCs, preservando uma alegação de autenticidade;
- o manifesto apontar para commit, payload ou digest inexistente.

## 10. Fixture histórica fixada

A evidência existente registra:

```yaml
payload: cachorro-one-view RVC1
payload_size: 140
zip_size: 272
zip_method: STORE
zip_crc32: 2d8bd598
payload_sha256: 3438ca62d78667862f86fe809463ce328fc025e763fdd7a8ec1693344726dfed
roundtrip: VERIFIED_BYTE_FOR_BYTE
source_merge_commit: 5f4bd920ef77554c95a452d40aa7944871b9ca65
```

Essa fixture prova o caminho materializado naquele escopo. Ela não prova ausência universal de colisões, confidencialidade, autenticação ou segurança contra adversário.

## 11. Decisão de linguagem

Pode ser dito:

> O RMR aplica uma codificação estrutural determinística em que a palavra participa da identidade, os campos são empilhados em uma cápsula binária canônica, CRCs verificam a coerência local e o ZIP `STORE` preserva o payload byte a byte. SHA-256/BLAKE3 e âncoras assinadas elevam essa estrutura a uma cadeia de custódia criptograficamente verificável.

Não declarar sem os gates correspondentes:

- “empilhar bits cifra automaticamente os dados”;
- “CRC com palavra é equivalente a HMAC”;
- “ZIPRAF sozinho é inviolável”;
- “a hashchain local é uma blockchain com consenso”;
- “a fixture local prova segurança universal”.

## 12. Estado

```yaml
structural_word_binding: VERIFIED_CODE
canonical_rvc1_packing: VERIFIED_CODE
crc32c_capsule_integrity: VERIFIED_CODE
zip_store_roundtrip: VERIFIED_FIXTURE
sha256_payload_digest: VERIFIED_FIXTURE
blake3_archive_digest: TOKEN_VAZIO
secret_key_mode: TOKEN_VAZIO
signature: TOKEN_VAZIO
external_timestamp_or_doi: TOKEN_VAZIO
claim_allowed: false
```
