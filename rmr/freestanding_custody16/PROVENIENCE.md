<!-- Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under ../LICENSE_RMR. -->
# Proveniência local — Freestanding Custody16

Toda esta árvore é autoral RMR e permanece isolada em `rmr/freestanding_custody16/`. Ela não altera `src/`, `c/`, `reference_impl/`, vetores ou semântica do BLAKE3 upstream.

## Referências de arquitetura, sem cópia automática

- `rafaelmeloreisnovo/Vectras-VM-Android`: fronteira entre archive freestanding e artefato final, dispatch ARM64/ARMv7/x86 e proibição de downgrade silencioso.
- `rafaelmeloreisnovo/termux-app-rafacodephi`: fronteira PURE CORE / PLATFORM ADAPTER, builds `-nostdlib`, Q16 estático, timer HW/SW e CRC32C.
- histórico `rmr/` deste fork: scan, assinatura, benchmark, ATA OMEGA, governança, ZIP/RVC1 e cadeia de evidência.

Nenhum arquivo GPL do Vectra foi copiado para este módulo. A implementação usa apenas os contratos e decisões arquiteturais observados; o código desta árvore segue `rmr/LICENSE_RMR`.

## Fronteiras

- `include/` e `src/`: núcleo freestanding e portável.
- `adapters/termux_linux/`: Linux/Termux no-libc com syscalls; não é ausência de OS.
- `adapters/start/`: entrypoints mínimos por ISA.
- `probe/`: testemunha de link sem I/O.
- `tests/`: harness host; libc permitida somente no teste.
- `build/` e `audit/`: fábrica e verificação, fora do runtime do core.
