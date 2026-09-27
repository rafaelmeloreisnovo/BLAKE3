<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Core Dependency Inventory V1

**Snapshot analisado:** `17f5dbaaea71dbc83825e21788b5adaddb6fb8ee`  
**Data:** 2026-09-26  
**Método:** inspeção code-first de `rmr/core/*.c` + dependência transitiva conhecida.

## Regra

Ausência de `#include <stdio.h>` não implica standalone.

A classificação considera:

```text
direct include
+ called API
+ transitive module
+ linker symbols
+ platform semantics
```

## Inventário

| Translation unit | Estado | Dependências/fronteira |
| --- | --- | --- |
| `base.c` | HOSTED_ADAPTER | parsing, strdup/free, FILE/report |
| `base_prime.c` | PURE_INTEGER_CORE_SOURCE | prev/next prime; sem I/O/heap |
| `bench.c` | HOSTED | processo, tempo, filesystem, métricas |
| `benchdiff.c` | HOSTED | FILE, qsort, relatório |
| `cli.c` | HOSTED_ROUTER | stdio/string + dispatch |
| `geom.c` | HOSTED_NUMERIC_APP | float/libm, heap, PGM/file I/O |
| `hash_sha256.c` | PURE_MEMORY_HASH_CORE | sem file I/O; SHA-256 autoral RMR |
| `hash_sha256_file.c` | HOSTED_FILE_ADAPTER | FILE/fopen/fread/fclose |
| `hash_blake3.c` | PURE_MEMORY_ADAPTER_TO_UPSTREAM | sem file I/O; depende da API BLAKE3 upstream |
| `hash_blake3_file.c` | HOSTED_FILE_ADAPTER | FILE/fopen/fread/fclose |
| `lowlevel_freestanding.c` | FREESTANDING_SUPPORT_SOURCE | arena somente quando macros específicas estão ativas |
| `main.c` | THIN_HOST_ENTRY | sem includes hosted diretos, mas chama router hosted |
| `scan.c` | HOSTED_FILESYSTEM_ADAPTER | dirent/stat/realloc/qsort/FILE |
| `sign.c` | HOSTED_EVIDENCE_RECORD_ADAPTER | FILE/env/time; não é assinatura digital PKI |
| `toroid.c` | HOSTED_NUMERIC_APP | float/libm, heap, PGM/OBJ I/O |
| `util.c` | HOSTED_FORWARDER | delega para pathcutter, que usa libc/OS |
| `validate.c` | HOSTED_VALIDATION_ADAPTER | parsing, stdout, log2/fabsf |
| `validate_core.c` | PURE_INTEGER_GATE_SOURCE | GCD + gates discretos; sem float/libc/I/O |

## Dependências hosted observadas

### Filesystem / streams

- `FILE`;
- `fopen/fread/fwrite/fclose`;
- `opendir/readdir`;
- `stat/lstat`.

### Heap/string/process

- `malloc/realloc/free`;
- `strdup/qsort`;
- `fork/wait` em benchmark;
- environment em rotas específicas.

### Tempo

- `time/gmtime` no `sign.c`;
- benchmark clocks/process timing nas rotas de benchmark.

### Matemática hosted

- `log2`, trigonometria, raiz e outras funções `libm` em superfícies geométricas/diagnósticas.

## Dependência transitiva importante

`main.c` é pequeno e sem libc direto, mas:

```text
main
 -> mode_router
 -> CLI/BBS
 -> hosted commands
```

Portanto:

```text
main.c no-hosted-includes != PAI freestanding
```

Da mesma forma:

```text
util.c
 -> pathcutter
 -> perror/exit/mkdir/libc
```

logo `util.c` não é promovido a pure core.

## Pure-core extraction já materializada neste ciclo

1. standalone foundation;
2. SHA-256 memory core separado de file adapter;
3. BLAKE3 bytes adapter separado de file adapter;
4. prime-neighbor integer core;
5. validation integer gate.

## Próxima ordem de extração

```text
P0 validate/base pure helpers
P1 scan canonical record consumer
P2 sign evidence-preimage serializer
P3 geometry fixed-point kernels
P4 toroid fixed-point kernel
P5 benchmark measurement core vs process adapter
P6 hosted CLI as optional shell only
```

## Gate

Um componente só sai de `*_SOURCE` para `FREESTANDING_PROVED` quando houver artifact/link receipt no alvo correspondente.
