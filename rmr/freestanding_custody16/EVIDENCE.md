<!-- Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under ../LICENSE_RMR. -->
# Evidência e proveniência técnica — Custody16

## Evidência histórica consolidada

A implementação junta, sem reescrever o núcleo BLAKE3:

1. warm-up/cache e benchmark estável do histórico do fork;
2. snapshot canônico por caminho/tamanho/conteúdo;
3. cadeia de integridade com CRC32C e SHA-256;
4. cápsula binária e ZIP STORE com round-trip;
5. metadados de hardware e temperatura fornecidos por adapter;
6. matriz 4×4 de 16 lanes e delta esparso bit a bit;
7. duas varreduras reais da árvore no adapter Termux/Linux e recusa quando A != B;
8. digest final com SHA-256 do selo, SHA-256 do ZIP e CRC32C do registro.

## Referências internas usadas como padrão

- Vectra separa archive freestanding e link probe final, com `-ffreestanding`, `-fno-builtin`, section GC, `-nostdlib`, `--build-id=none`, `--no-undefined` e entrypoint explícito.
- Vectra seleciona ARM64 `+crc+simd`, ARMv7 NEON/softfp e x86 SSE4.2, proibindo downgrade silencioso do core.
- Termux documenta a fronteira `PURE CORE / PLATFORM ADAPTER`: filesystem, relógio e thread ficam fora do core.
- O benchmark Termux histórico usa Q16, estado estático alinhado, timer HW/SW e CRC32C SW.

## Claims e falsificadores

| Claim | Estado | Falsificador |
|---|---|---|
| artefato sem dependência dinâmica | verificável pelo probe | `DT_NEEDED`, `PT_INTERP` ou UND inesperado |
| alteração esparsa muda só bits divergentes | selftest | qualquer palavra não listada mudar |
| ZIP reproduz payload byte a byte | selftest | tamanho, CRC-32 ou byte divergente |
| CRC32C HW/SW mantém semântica | KAT pendente por arquitetura | resultado diferente no mesmo vetor |
| 16 lanes mantêm resultado canônico | contrato implementado | redução depender da ordem de execução |
| duas varreduras detectam mutação | adapter implementado | A e B divergirem sem rejeição |
| digest final cobre selo e ZIP | adapter implementado | SHA-256 ou CRC32C divergente |
| snapshot de diretório é OS-free | **não permitido** | diretório exige adapter/plataforma |
| assinatura de hardware é PUF físico | **não reivindicado** | falta de ensaio físico e unicidade |

`claim_allowed=false` para PUF, superioridade universal, ausência total de OS no adapter Termux e escalabilidade multicore até execução materializada em hardware.
