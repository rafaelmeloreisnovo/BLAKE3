<!-- Copyright (c) 2024-2026 Rafael Melo Reis. Licensed under ../../LICENSE_RMR. -->
# Verificação local materializada

Data de execução: 2026-07-27. Ambiente de auditoria: host x86_64 com Clang/LLD. Os números abaixo demonstram funcionamento, não superioridade universal.

## Selftest

```text
FC16_OK seal=376 zip=500 root=714fc950 flags=00000058 deltas=2
```

O teste inclui KAT CRC32C `123456789 = e3069283`, CRC-32 IEEE `cbf43926`, SHA-256 de `abc`, delta esparso, selo CRC32C e ZIP round-trip.

## Matriz de link

```text
[OK] /tmp/rmr_fc16_x86_64
[OK] /tmp/rmr_fc16_aarch64
[OK] /tmp/rmr_fc16_armv7
```

Os três artefatos foram produzidos com `-nostdlib -static --gc-sections --build-id=none --no-undefined`. `readelf` não encontrou `PT_INTERP`, `DT_NEEDED` ou UND inesperado.

## Fixture de árvore

Fixture final: três arquivos, um diretório aninhado, 13 bytes de conteúdo e quatro entradas totais incluindo o diretório.

```text
FC16_OK
root_crc32c=0xa476b0d7
entries=4
bytes=13
seal_bytes=376
zip_bytes=500
digest_bytes=76
```

Os tempos de scan variam por ambiente e não são vetor ouro. O vetor estrutural é o root/contagem/conteúdo sob a mesma fixture e build.
