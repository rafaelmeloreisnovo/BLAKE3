<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Release Notes — 2026-09-26

## Tipo

Documentation / architecture hardening.  
Não é release de produção.

## Contexto

A documentação histórica misturava, em alguns pontos, intenção de baixo nível com o estado real do executável PAI.

A inspeção code-first mostrou:

- PAI CLI/scan/sign ainda dependem de libc/POSIX;
- o BLAKE3 adapter de bytes é estreito, mas file I/O é hosted;
- crypto runtime possui provider OpenSSL opcional;
- Custody16 já possui core freestanding e prova local de link;
- FIXED256 já contém kernels ASM bare-metal;
- PAI42 tem core C Q16 separado de bridge Python;
- tempo forense é sidecar hosted e deliberadamente não faz parte do core sem relógio.

## Added

- `00_START_HERE_RMR.md`;
- `RMR_IMPLEMENTATION_STATUS_V1.md`;
- `RMR_STANDALONE_ARCHITECTURE_V1.md`;
- `RMR_BUILD_AND_BINARY_CONTRACT_V1.md`;
- `RMR_ROADMAP_V1.md`;
- este release note.

## Corrected

- “bare metal” não é mais usado como classificação do PAI completo.
- comentário comum não é tratado como mecanismo de redução de binário.
- Q8/Q16/Q32/Q42 são classificados como perfis fixed-point.
- “sign” histórico é separado de assinatura digital assimétrica/PKI.
- hosted/provider/freestanding são estados distintos.

## Preserved

- núcleo BLAKE3 upstream;
- licenças e notices upstream;
- receipts existentes;
- `TOKEN_VAZIO`;
- append-only/supersession;
- interfaces históricas.

## Risk

Nenhum código de produção foi alterado neste primeiro commit documental.

## Next

Adicionar uma foundation freestanding isolada com tipos compiler-native, memory primitives, perfis Q e cross-link proof. Somente depois migrar funções do PAI por módulo.
