<!-- Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under ../LICENSE_RMR. -->
# RMR Freestanding Custody16

Núcleo externo ao BLAKE3 upstream para custódia determinística de snapshots fornecidos por memória.

## Contrato

- C11 freestanding; sem libc, heap, GC, POSIX, Android, JNI, filesystem, relógio ou threads no core.
- 16 lanes fixas organizadas em matriz 4×4; cada bloco de 16 bytes pertence a um slot geométrico estável.
- redução final em ordem canônica 0..15, independentemente da ordem física usada pelo adapter;
- NEON em ARM, SSE2 em x86 e fallback escalar;
- CRC32C ARMv8/x86 SSE4.2 quando compilado para essas extensões; fallback SW Castagnoli;
- SHA-256 interno e CRC32C do selo;
- ZIP method 0 `STORE`, CRC-32 IEEE e verificação byte a byte;
- delta esparso `word_index + xor_mask`: somente bits diferentes são invertidos.

## Fronteira real

Um diretório é conceito do sistema operacional. Por isso, o core recebe `path + size + bytes` já canonizados. O adapter `adapters/termux_linux/` usa syscalls diretas fora do core, ordena cada diretório, exclui os três artefatos autorreferentes e alimenta o núcleo. Vectra/JNI ou QEMU devem implementar a mesma interface, sem copiar a matemática. Isso mantém a matemática reaproveitável em Termux, QEMU user, QEMU system, firmware e RAM bruta.

## Paralelismo

As 16 lanes não significam 16 threads obrigatórias. Elas são 16 unidades independentes com slots fixos. Um adapter pode executá-las em até 16 workers sem alterar o resultado; NEON/SSE processa 16 bytes por instrução em lockstep. Multicore e SIMD permanecem camadas distintas.

## Build

```sh
./rmr/freestanding_custody16/build/build_host_selftest.sh
./rmr/freestanding_custody16/build/build_freestanding_probe.sh
./rmr/freestanding_custody16/build/build_termux_snapshot.sh
./rmr/freestanding_custody16/build/build_cross_matrix.sh
```

O probe final exige: `-nostdlib`, nenhum `DT_NEEDED`, nenhum `PT_INTERP` e nenhum símbolo indefinido inesperado.

## Estado

- core, CRC32C HW/SW, SIMD, SHA-256, selo, ZIP STORE e delta esparso: `IMPLEMENTADO`;
- adapter Termux/Linux no-libc: `IMPLEMENTADO` com `openat/getdents64/statx/read`, duas varreduras, gate térmico opcional e saídas `custody16.bin`, `custody16.zip`, `custody16.digest`;
- fallback sem `statx`: usa `d_type` e contagem prévia do arquivo;
- paralelismo SIMD de 16 bytes: `IMPLEMENTADO`; paralelismo multicore real de 16 workers: `TOKEN_VAZIO_ADAPTER`;
- leitura térmica/hardware: metadados são injetados pelo adapter; o core apenas aplica o gate determinístico.
