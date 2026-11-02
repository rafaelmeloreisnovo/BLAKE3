<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
AI-assisted technical provenance draft; human review required before promotion.
-->

# RMR — cadeia de proveniência criptográfica (ChaCha20-Poly1305, Ed25519, Ed448, SHA-256, SHA-512)

**Estado:** DRAFT / HOLD_FOR_HUMAN_REVIEW / claim_allowed=false
**Data do levantamento:** 2026-11-02 (instante UTC e relógio externo: TOKEN_VAZIO)
**Repositório produtor:** `rafaelmeloreisnovo/BLAKE3`
**Commit de origem fixado:** `365516a302f23f94c538c3a27e3d26e3d294098e` (branch `master` no levantamento)
**Tree desse commit:** `5ba79c50428c7f7a6ac5767057be5be492631079`
**Escopo:** contrato, registro, interfaces de código, testes-fonte, direitos e lacunas. NÃO altera primitivas, vetores, API, build ou execução.

## 1. Semântica da proveniência

```text
ALGORITHM_STANDARD != IMPLEMENTATION != RMR_ADAPTER != PROVIDER_BINARY
GIT_BLOB_SHA != SHA256(file bytes) != RUNTIME_ARTIFACT_SHA256
SOURCE != ARTIFACT != EXECUTION != EVIDENCE != CLAIM
REGISTRY_STATUS(IMPLEMENTED_PROVIDER) != exact-head KAT PASS
INVENTORY / AUTHORED_ADAPTER != AUTHORSHP_OF_CRYPTOGRAPHIC_ALGORITHM
TOKEN_VAZIO != 0
```

O BLAKE3 oficial é **upstream** (`BLAKE3-team/BLAKE3`); o fork mantém código herdado e acrescenta uma camada separada `rmr/`. Os algoritmos abaixo não foram inventados pelo RMR. SHA-2 e EdDSA são especificações públicas; implementações OpenSSL têm autoria/licença de seus respectivos contribuidores. A autoria declarada do RMR cobre apenas a organização, documentação, adaptadores e o código efetivamente criado no projeto, sujeita à prova arquivo a arquivo.

As referências normativas **não** são cópias de código e **não** equivalem a uma licença do provider. OpenSSL 3, versão exata, build, licença por arquivo, vinculação, artefato e dispositivo usados numa futura execução devem constar do receipt daquele ensaio.

## 2. Matriz de dependências e atribuição

| Endereço RMR (contrato) | Especificação / origem técnica | Implementação real no runtime | Escopo de autoria RMR | Teste-fonte e falsificador |
| --- | --- | --- | --- | --- |
| `rmr/crypto/algorithms/chacha20-poly1305/README.md` | ChaCha20/Poly1305; RFC 8439 | `rmr_crypto_openssl.c` → `EVP_chacha20_poly1305()` (OpenSSL 3, `EVP_CIPHER`) | wrapper, seleção, contratos, teste; **não** o algoritmo ChaCha ou OpenSSL | RFC 8439 ciphertext/tag e decrypt roundtrip; falsificar AAD, ciphertext, tag, nonce e reutilização de nonce (protocol-level) |
| `rmr/crypto/algorithms/ed25519/README.md` | EdDSA/Ed25519; RFC 8032 | `rmr_crypto_openssl.c` → `EVP_PKEY_ED25519` | wrapper seed→public/sign/verify; **não** EdDSA/OpenSSL | RFC 8032 vetor, assinatura modificada, chave pública errada |
| `rmr/crypto/algorithms/ed448/README.md` | EdDSA/Ed448; RFC 8032 | `rmr_crypto_openssl.c` → `EVP_PKEY_ED448` | wrapper seed→public/sign/verify; **não** EdDSA/OpenSSL | RFC 8032 vetor de mensagem vazia, assinatura modificada |
| `rmr/crypto/algorithms/sha256/README.md` | SHA-2/SHA-256; NIST FIPS 180-4 | provider `EVP_sha256()` **e, separadamente**, `rmr/core/hash_sha256.c` e `rmr/freestanding_custody16/src/rmr_fc16_sha256.c` | adaptação e **implementações locais C de SHA-256** (atribuição por evidência de commits; não reivindicar o algoritmo SHA-256) | `abc`, vazio, streaming, limites 55/56/63/64/65, comparação entre três rotas |
| `rmr/crypto/algorithms/sha512/README.md` | SHA-2/SHA-512; NIST FIPS 180-4 | `rmr_crypto_openssl.c` → `EVP_sha512()` | interface, registro e testes; nenhuma implementação standalone de SHA-512 foi identificada nesta rota | `abc`, vazio, 111/112/127/128/129, comparação com oracle independente |

**Precisão sobre a família ED:** `Ed25519` e `Ed448` são duas instâncias distintas de EdDSA. Não existe um algoritmo `Ed2588-591` confirmado na árvore observada. `X25519`/`X448` são perfis de **acordo de chaves**, não assinaturas ED; não confundir nomenclaturas.

Referências primárias:
- ChaCha20/Poly1305: https://www.rfc-editor.org/rfc/rfc8439
- Ed25519/Ed448: https://www.rfc-editor.org/rfc/rfc8032
- SHA-256/SHA-512: https://csrc.nist.gov/pubs/fips/180-4/upd1/final
- Autoridade BLAKE3: https://github.com/BLAKE3-team/BLAKE3

## 3. Identidade verificável dos arquivos no snapshot

Todos abaixo são **Git blob SHA-1** observados no commit fixado; **não** são digests SHA-256 de arquivos nem prova de autoria.

| Caminho | Git blob SHA |
| --- | --- |
| `rmr/crypto/runtime/registry.json` | `0a34f8dda76397f58029a018334d31c841c07cbf` |
| `rmr/crypto/runtime/src/rmr_crypto_openssl.c` | `2673e50e6727c4ecaf77295116db6776552819d6` |
| `rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c` | `d459f7c1d5ab3fd44c809dbab65e749297232612` |
| `rmr/crypto/algorithms/chacha20-poly1305/README.md` | `fc915de5e1793c2050258c7b14f8b838dcd42408` |
| `rmr/crypto/algorithms/ed25519/README.md` | `2d899b1e0a5d9f40426cdaf1db4214cb1134c7c5` |
| `rmr/crypto/algorithms/ed448/README.md` | `027ec7d3ae208d8a8c5dca81dca43da3200b116a` |
| `rmr/crypto/algorithms/sha256/README.md` | `0f46c1db0b8fbf8b212b98c7dbf66c8ae86f84f9` |
| `rmr/crypto/algorithms/sha512/README.md` | `16ed471baf2700a6a4f1b6c9d5dd337f2e8cf530` |
| `rmr/core/hash_sha256.c` | `1946a09612e0e6249d457d4ab8a26f983c787b51` |
| `rmr/freestanding_custody16/src/rmr_fc16_sha256.c` | `99ae38672e55b0c7fdf6b20cb04b70aca40e5403` |
| `rmr/crypto/runtime/src/rmr_crypto_blake3.c` | `dd73a47d42f284da03b49d0b139496d246d458a5` |
| `rmr/crypto/runtime/receipts/RMR_CRYPTO_V3_PROVIDER_CROSSCHECK_20260924.json` | `01a335b8eac87a32c0e0dfc5156fec3d615a03da` |

Apenas como contexto de BLAKE3: `rmr/crypto/runtime/src/rmr_crypto_blake3.c` chama a API BLAKE3 existente em `c/`; não reimplementa a primitiva. Nenhuma declaração de autoria RMR sobre código upstream é permitida.

## 4. Evidência positiva, negativa e regras de promoção

**Observação verificada neste levantamento:** arquivos, caminhos, blobs e presença de mapeamentos em `registry.json` e chamadas reais de OpenSSL em `rmr_crypto_openssl.c`; vetores conhecidos codificados em `rmr_crypto_runtime_selftest.c`.

**Limitação:** a presença de um teste no source não comprova que foi executado no commit `365516a...`. O receipt histórico de 2026-09-24 (`RMR_CRYPTO_V3_PROVIDER_CROSSCHECK_20260924.json`) é sobre cinco OUTROS algoritmos SHA-512/224, SHA-512/256, SHA3-224, SHA3-384 e SM3, num HEAD anterior; não constitui PASS dos cinco alvos deste levantamento.

| Gate | Estado | Teste terminal / condição de parada |
| --- | --- | --- |
| Identidade dos caminhos/blobs | OBSERVED_AT_PINNED_SOURCE | refazer `git ls-tree -r` no commit fixado e comparar todos os blobs acima; STOP quando exato |
| Algoritmo→provider→wrapper | SOURCE_REVIEW_SCOPED | inspeção das chamadas reais, registry e sem vendoring indevido; STOP sem ambiguidade |
| ChaCha20-Poly1305 | KAT_PRESENT_NOT_RERUN | RFC 8439 exato, tag/AAD/nonce mutações; falha fechada; STOP com receipt por provider |
| Ed25519 e Ed448 | KAT_PRESENT_NOT_RERUN | RFC 8032, negativos, mensagens alteradas; STOP com receipt por provider |
| SHA-256 provider + duas rotas locais C | NOT_RERUN_CROSS_ROUTE | KAT/boundaries/streaming e igualdade byte a byte; STOP por ABI + provider |
| SHA-512 | KAT_PRESENT_NOT_RERUN | `abc` e fronteiras SHA-512; STOP com oracle/receipt |
| CI exato neste HEAD | TOKEN_VAZIO | conferir run, job, conclusão e checkout SHA exato |
| Provider OpenSSL exato + licença de distribuição | TOKEN_VAZIO | versão/binário/libcrypto, arquivo LICENSE e notices por commit |
| Android ARMv7/ARM64 físico e equivalência | NOT_RUN_THIS_SCOPE | KATs e hashes do artefato, CPU/ABI/dispositivo, logs assinados/hasheados |
| Criptografia certificada ou novidade autoral da primitiva | NOT_CLAIMED | nenhum claim sem avaliação independente apropriada |

Para ensaio futuro (somente quando autorizado), a rota já documentada é:

```sh
python3 rmr/crypto/runtime/tools/validate_runtime_registry.py
sh rmr/crypto/runtime/build/build_contract_matrix.sh
cmake -S rmr -B build/rmr-crypto -G Ninja -DRMR_BUILD_TESTS=ON -DRMR_BUILD_CRYPTO_RUNTIME=ON
cmake --build build/rmr-crypto --target rmr-crypto-runtime-selftest
ctest --test-dir build/rmr-crypto -R rmr_crypto_runtime_selftest --output-on-failure
```

Registrar **antes** da promoção: exact-head, clang/gcc, flags, target, libcrypto version/sha, binary SHA-256, result + exit status, vectors e negativos, runner, data UTC, hardware e eventual no-libc linkage. Cross compile não é execução física; CI verde de BLAKE3 não valida OpenSSL.

## 5. Custódia, direitos e rollback

- Nenhum arquivo de terceiros foi importado nesta alteração documental; a licença de `rmr/LICENSE_RMR` e as licenças oficiais BLAKE3 permanecem intocadas.
- Conteúdo RMR neste relatório é **documentação nova**; direitos dos protocolos, especificações, implementações e marcas permanecem com os respectivos titulares.
- Classificações `RMR autoral` fora dos adaptadores e arquivos explicitamente revisados permanecem DECLARED / TOKEN_VAZIO por arquivo. Git autoria e copyright devem ser revisados por humano.
- O próprio repositório declara que decisão sobre licença, compatibilidade ou promoção requer revisão humana; esta matriz **não** altera termos jurídicos.
- A restauração consiste em fechar o PR sem merge e/ou reverter exclusivamente os commits aditivos; nunca reescrever árvores/receipts preexistentes.
- `CLAIM_ALLOWED=false`, `MERGE_ALLOWED=false` até revisão independente, checks e gate de autoridade.

## 6. R3

`F_ok`: origem técnica, wrappers, interfaces, blobs e testes-fonte mapeados no commit observado.

`F_gap`: provider executado exatamente, resultado KAT neste HEAD, negativos adicionais, evidência física, licenças por arquivo, comparação independente e segurança de uso = TOKEN_VAZIO/NOT_RUN.

`F_next`: auditoria exata da matriz, KAT/negativos em branch isolada, receipt sha256 por binário e revisão humana antes de qualquer promoção.
