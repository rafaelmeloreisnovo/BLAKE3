# Ecossistema RMR/Rafaelia — ponteiro cruzado (2026-07-03)

Status: `PENDING` (documentação de ponteiro; nenhuma integração de código)

Este repositório é um fork/redistribuição do BLAKE3 oficial
(`BLAKE3-team/BLAKE3`, confirmado em `README.md` e `FORK_NOTES.md`), com uma
camada isolada `rmr/` — "Blacktrain": cadeia de custódia e mensuração de
build/benchmark do binário BLAKE3 (`rmr/reports/RMR_ARTIFACTS_SHA256.txt`,
`rmr/build/profiles.mk`, `docs/RAFAELIA_BLACKTRAIN_READY_STATUS.md`), não
uma extensão criptográfica do algoritmo em si — `rmr/PROVENIENCE.md` já
separa explicitamente "núcleo BLAKE3 upstream" de "RMR autoral".

Até esta rodada, este repositório **não referenciava nenhum repositório
irmão**, apesar de ser citado por dois deles:

- `llamaRafaelia/docs/RAFAELIA_VECTRA_LAMA_CONNECTOR_BRIDGE.md` nomeia este
  repositório como `"Blacktrain — binário/hash/mensuração/custódia"`.
- `RafPolimata/docs/LICENCAS_COMPARADAS.md` usa BLAKE3 como referência de
  licenciamento permissivo.

## Documento central do ecossistema

Tabela de evidência entre os seis repositórios investigados
(`RafPolimata`, `llamaRafaelia`, `GAIA_phi`, `CONVERSATIONS_CHUNKS_PRIVATE`,
`ChipQuantum`, `BLAKE3`), divergências de nomenclatura e costuras de módulo
candidatas estão em:

> `rafaelmeloreisnovo/RafPolimata` → `docs/CONVERGENCIA_ECOSSISTEMA_RMR_RAFAELIA.md`

## Nota sobre a camada Fiber H (deste próprio repositório)

`docs/rafaelia/RMR_LICENSE_FIBER_H_BOUNDARY.md` e
`RMR_BLAKE3_BINARY_ORCHESTRATION_VS_FIBERHAGA.md` já descrevem "Fiber H" /
"FiberHaga" como um núcleo de hashing autoral **separado** do BLAKE3 e ainda
não ligado a nenhum repositório irmão. Vale notar, para uma rodada futura,
que `RafPolimata/rafaelia/fiber_h.h` implementa um "Fiber-H (256-bit Hamming
hash)" como parte do motor `verbovivo` — mesmo nome, repositórios diferentes,
sem confirmação de que sejam o mesmo conceito. Registrado como `TOKEN_VAZIO`
até uma comparação linha a linha ser feita.
