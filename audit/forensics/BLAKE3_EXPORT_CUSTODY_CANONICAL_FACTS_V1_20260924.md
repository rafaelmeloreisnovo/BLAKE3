# BLAKE3 / Export Custody — Fatos Canônicos V1

**Snapshot:** 2026-09-24  
**Custódia:** `rafaelmeloreisnovo/BLAKE3` + Google Drive privado + Gmail privado  
**Classe:** `FORENSIC_EXPORT_CUSTODY / NON_ACCUSATORY / FAIL_CLOSED`

## Regra

`SOURCE != ARTEFATO != EXECUÇÃO != EVIDÊNCIA != CLAIM`

Este artefato preserva somente fatos verificáveis. Ele não afirma adulteração, fraude,
plágio, sabotagem, rehash deliberado, coordenação por bots ou finalidade estratégica
sem ponte probatória específica.

## 1. E-mails de exportação

Amostras autenticadas pelo cabeçalho Gmail/MIME foram lidas com `DKIM=PASS`,
`SPF=PASS` e `DMARC=PASS` para o domínio remetente observado.

A sequência amostral de endpoints é:

`2025-03-05 backend-api/content`
→ `2025-07-09 backend-api/estuary/content`
→ `2025-07-10 backend-api/content`
→ `2025-07-11 backend-api/estuary/content`
→ `2025-10-06 backend-api/estuary/content`
→ `2025-12-31 backend-api/estuary/content`

Isso prova transição/coexistência de rotas de entrega no período observado.

Cada e-mail contém um `zip_id` distinto com timestamp embutido e identificador
único. Isso prova materialização de artefatos de exportação distintos. Não prova,
por si só, alteração do conteúdo histórico.

Os identificadores pessoais e cabeçalhos brutos permanecem no Drive privado.
O ledger público usa somente digests desses identificadores.

## 2. Drive / NOVOexport já possui cadeia de custódia canônica

A camada privada existente já estabelece:

- `fileId` como identidade primária do objeto Drive;
- rename != mudança semântica;
- filename similarity != content identity;
- hash equality pode sustentar identidade byte-a-byte;
- digest só pode ser declarado quando realmente calculado;
- `claim_allowed=false` enquanto o gate correspondente não estiver fechado.

Evidência existente:
- 39 objetos DOCX/DAT com SHA-256 byte-backed;
- RAW018: 12.115.336 bytes, SHA-256
  `3cf4783727feb5c53868af76d6e2de660e2287c2cb075180a6aad2738b10140c`,
  JSON parse PASS, 100 objetos;
- corpus 000..050: 51/51 shards, 1.107.289.897 bytes, 5.054 objetos;
- witness histórico 2025-09-12: archive 258.114.346 bytes,
  SHA-256 `0d1eec04eea3b3a8e3cf6435b6a6fb0e732db3f037ac9220f09031b0ecba8815`,
  `conversations.json` 678.116.200 bytes descomprimidos, 2.334 conversas;
- ALL_TOKEN: stores detalhados concordam em 44.185.627 ocorrências e o manifest
  congelado contém 44.185.626; generator/root cause permanece `TOKEN_VAZIO`.

## 3. Hashing do pacote atual

O receipt multi-hash acompanha os arquivos públicos deste pacote com:

- MD5 (legado, não usado como garantia de segurança);
- SHA-1 (legado);
- SHA-256;
- SHA-512;
- SHA3-256;
- SHA3-512;
- BLAKE2s-256;
- BLAKE2b-512;
- BLAKE3-256.

`Ed25519` não foi fabricado. Sem chave canônica previamente registrada:
`ED25519_SIGNATURE=TOKEN_VAZIO_NO_CANONICAL_SIGNING_KEY`.

## 4. O que está provado

- existem múltiplas exportações materializadas em datas distintas;
- em amostra de julho de 2025, as rotas `content` e `estuary/content` coexistem;
- os cabeçalhos observados passam DKIM/SPF/DMARC;
- cada artefato de exportação observado usa identificador ZIP distinto;
- o Drive contém cadeia de custódia e hashes SHA-256 de fontes privadas;
- RAW018 possui witness byte-backed e SHA-256 fechado;
- existe um drift escalar `44.185.626` versus `44.185.627` já preservado como
  evidência, com causa-raiz ainda não localizada.

## 5. O que NÃO está provado

- que mudanças de endpoint impliquem adulteração;
- que identificadores ZIP distintos impliquem modificação de conversas;
- que algum atraso de até uma semana tenha sido usado para alterar dados;
- que BLAKE3/libc/robôs sejam usados para reencaixar deliberadamente registros;
- que o processo de exportação da OpenAI tenha nexo causal com upstreaming do BLAKE3.

Esses pontos permanecem `TOKEN_VAZIO` até comparação byte/objeto entre exports
ou outra evidência independente.

## 6. Próximo falsificador

Para testar a hipótese de reempacotamento/reescrita de conteúdo:

`export_n -> conversations.json -> conversation_id -> create_time/update_time -> canonical object serialization -> multi-hash`

versus:

`export_n+1 -> mesmos IDs -> mesmos campos -> multi-hash`

Mudança de ZIP/URL/assinatura externa não é suficiente. O teste deve ocorrer nos
mesmos objetos internos.
