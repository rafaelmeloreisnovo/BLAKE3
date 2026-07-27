<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Módulo RMR

Todo código e documentação autoral sob `rmr/` é licenciado conforme `rmr/LICENSE_RMR`, salvo exceção de terceiro nominalmente registrada em `rmr/PROVENIENCE.md`.

O código BLAKE3 upstream permanece sob suas licenças, autoria, notices e semântica originais. RMR não reivindica autoria sobre BLAKE3 nem converte automaticamente a licença do upstream.

## Fronteiras normativas

1. `rmr/LICENSE_RMR` — texto jurídico do material autoral RMR.
2. `rmr/PROVENIENCE.md` — origem, autoria, licença e exceções por caminho.
3. `rmr/docs/ARCHITECTURE.md` — arquitetura, isolamento e cabeçalhos canônicos.
4. `rmr/crypto/README.md` — catálogo criptográfico, perfil SHA-256 e governança de referências.
5. `rmr/crypto/AUTHORSHIP.md` — atribuição, assistência por IA e responsabilidade humana.
6. `rmr/crypto/CONTRIBUTING.md` — protocolo de contribuição e importação de terceiros.
7. `rmr/crypto/THIRD_PARTY_NOTICES.md` — notices, marcas e não afiliação.
8. `rmr/crypto/SECURITY.md` — limites de segurança e reporte.
9. `rmr/pai42/README.md` — ponte geométrica determinística entre os 42 ciclos ATA OMEGA e a observação circular PAI42.

## RMR Crypto Registry

`rmr/crypto/` cataloga dez famílias de referência e três candidatos relacionados por família. O catálogo é `reference_only`: ele não incorpora automaticamente código externo, não confirma parentesco de fork por nome e não declara compatibilidade de licença sem auditoria fixada em commit.

Validação local:

```sh
./rmr/crypto/tools/audit_crypto_registry.sh
```

A validação comprova coerência estrutural do snapshot. Ela não equivale a certificação criptográfica, jurídica ou de segurança.

## RMR PAI42

`rmr/pai42/` recebe exatamente 42 registros válidos do `ATA_OMEGA.bin`, normaliza os ciclos em Q16 e produz uma projeção circular com raio, coordenadas, variância, simetria, estabilidade e escore de protótipo.

O núcleo é C de tamanho fixo, sem heap e sem ponto flutuante. A ferramenta Python é `stdlib-only` e serve como leitor/auditor fora do hot path. A estrutura não modifica o algoritmo BLAKE3 nem reivindica que `HW_SIG64` seja PUF ou serial físico único.

Validação local:

```sh
sh rmr/pai42/tests/run_tests.sh
```

## Conteúdo jurídico versus não jurídico

Conteúdos manifestários, conceituais ou técnicos não devem ser inseridos dentro de `rmr/LICENSE_RMR`. Esses materiais permanecem em documentos próprios, incluindo `rmr/MANIFESTO_RAFAELIA.md` e os documentos de arquitetura.

## Regra humana

Nenhuma automação pode alterar termos de licença, atribuir autoria, importar terceiro ou promover claim jurídico sem revisão humana expressa. Quando faltar evidência, registrar `TOKEN_VAZIO` e o próximo passo verificável.
