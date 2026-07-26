<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Referências e notices de terceiros

## 1. Natureza do catálogo

O registro em `rmr/crypto/registry/architectures.json` contém nomes e identificadores públicos de repositórios para pesquisa, comparação e auditoria. **Nenhum código desses projetos é copiado por este módulo.**

```yaml
third_party_code_in_rmr_crypto: false
reference_only: true
affiliation_claimed: false
endorsement_claimed: false
license_compatibility_review_completed: false
```

## 2. Projetos identificados

- OpenSSL — `openssl/openssl`
- BoringSSL — `google/boringssl`
- AWS-LC — `aws/aws-lc`
- Mbed TLS — `Mbed-TLS/mbedtls`
- wolfSSL — `wolfSSL/wolfssl`
- OpenSSH Portable — `openssh/openssh-portable`
- libsodium — `jedisct1/libsodium`
- BLAKE3 — `BLAKE3-team/BLAKE3`
- liboqs — `open-quantum-safe/liboqs`
- Crypto++ — `weidai11/cryptopp`

Os nomes, marcas, copyrights e códigos pertencem a seus respectivos titulares e contribuidores.

## 3. Licenças

A presença de um projeto no catálogo não declara compatibilidade com `rmr/LICENSE_RMR`. A licença precisa ser verificada no commit exato antes de qualquer uso que ultrapasse mera referência.

Estado padrão:

```text
TOKEN_VAZIO_NOT_REVIEWED
```

A revisão deve distinguir:

- licença do repositório principal;
- licença por subdiretório/arquivo;
- exceções de linking;
- notices obrigatórios;
- patentes;
- marcas;
- dependências transitivas;
- mudanças de licença entre commits.

## 4. BLAKE3 neste fork

As cópias das licenças upstream existentes na raiz — como `LICENSE_A2`, `LICENSE_A2LLVM` e `LICENSE_CC0` — permanecem aplicáveis aos respectivos componentes upstream. `rmr/LICENSE_RMR` aplica-se somente ao material autoral classificado como RMR.

Nenhum texto deste diretório reduz, substitui ou reinterpreta as licenças upstream.

## 5. Requisitos futuros de vendoring

Se material de terceiro for incorporado, adicionar antes do merge:

1. entrada nominal neste documento;
2. repositório, SHA e caminho de origem;
3. licença e notice copiados conforme obrigação;
4. lista de arquivos importados;
5. lista de modificações locais;
6. hashes antes/depois;
7. conclusão humana de compatibilidade ou `TOKEN_VAZIO` bloqueante;
8. atribuições em distribuições binárias quando exigidas.

## 6. Relações de fork

Os três candidatos associados a cada arquitetura foram catalogados como alvos de investigação. A relação direta com o upstream só é confirmada quando metadados de parentesco forem preservados.

```text
nome coincidente ≠ fork confirmado
história semelhante ≠ derivação comprovada
fork no GitHub ≠ licença automaticamente compatível
```

## 7. Ausência de aconselhamento jurídico

Este documento é uma política técnica de proveniência e não substitui análise jurídica profissional. Dúvidas materiais permanecem `TOKEN_VAZIO_LEGAL_REVIEW`.