<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Perfil SHA-256 do RMR

## 1. Papel

SHA-256 é usado no RMR como digest de interoperabilidade e cadeia de custódia. O backend atual é exposto por `rmr/core/pai_hash.h`.

```yaml
algorithm: SHA-256
output_bits: 256
block_bytes: 64
rmr_role: custody_digest
password_hash: false
digital_signature: false
message_authentication_without_keyed_construction: false
certification_claimed: false
```

## 2. Fronteira com BLAKE3

- SHA-256 não é BLAKE3.
- BLAKE3 não é rotulado como SHA-256.
- O uso de ambos no mesmo manifesto produz digests independentes.
- Nenhum digest isolado prova autoria, identidade, licença ou legalidade.

## 3. Vetores conhecidos mínimos

```text
SHA256("")
= e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

SHA256("abc")
= ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

Esses vetores são gates mínimos de sanidade. Passá-los não equivale a certificação formal da implementação.

## 4. Serialização de custódia

Quando um manifesto for hashed, a entrada deve ser definida por bytes, não por objeto abstrato. Para JSON:

1. UTF-8;
2. chaves em ordem lexicográfica;
3. separadores compactos `,` e `:`;
4. sem NaN/Infinity;
5. newline final explicitamente declarado;
6. versão de canonicalização registrada.

Formato recomendado:

```yaml
canonicalization: rmr-json-c14n-v1
encoding: UTF-8
newline: LF_FINAL
hash_algorithm: SHA-256
```

## 5. Domínios de uso

Permitido:

- fingerprints de arquivos e logs;
- manifestos de execução;
- listas de artefatos;
- verificação de igualdade de bytes;
- compatibilidade com ferramentas existentes.

Não permitido como solução isolada:

- armazenamento de senha;
- assinatura digital;
- autenticação de origem;
- proteção contra alteração por agente que também pode reescrever o manifesto;
- prova jurídica conclusiva de autoria.

## 6. API RMR

```c
pai_sha256_ctx ctx;
uint8_t digest[32];
char hex[65];

pai_sha256_init(&ctx);
pai_sha256_update(&ctx, data, len);
pai_sha256_final(&ctx, digest);
pai_sha256_hex(digest, hex);
```

Requisitos:

- aceitar atualização incremental;
- produzir exatamente 32 bytes;
- produzir hexadecimal minúsculo de 64 caracteres mais `NUL`;
- não ler além do buffer;
- não depender de estado global mutável;
- documentar comportamento para ponteiro nulo e tamanho zero;
- preservar resultado entre alvos.

Os últimos dois itens permanecem sujeitos a teste específico quando não cobertos por evidência atual.

## 7. Testes obrigatórios

- KAT vazio;
- KAT `abc`;
- atualização byte a byte versus bloco único;
- limites 55, 56, 63, 64, 65 bytes;
- arquivo vazio;
- arquivo maior que um bloco;
- repetibilidade;
- comparação com implementação independente;
- sanitizers em host quando disponíveis;
- ARMv7/AArch64/x86_64 quando houver runner.

## 8. Estado

```yaml
api_present: VERIFIED_PRIMARY
empty_kat_in_registry_auditor: IMPLEMENTED
abc_kat_in_registry_auditor: IMPLEMENTED
full_boundary_matrix: TOKEN_VAZIO
cross_architecture_equivalence: TOKEN_VAZIO
formal_validation_certificate: NOT_CLAIMED
```