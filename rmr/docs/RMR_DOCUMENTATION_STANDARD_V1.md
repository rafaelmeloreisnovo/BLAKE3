<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Documentation Standard V1

## Objetivo

Todo módulo RMR deve permitir que um engenheiro, auditor ou mantenedor responda, sem depender da memória do autor:

1. o que é;
2. onde está o código;
3. qual estado real;
4. como compila;
5. como testa;
6. de que depende;
7. que evidência existe;
8. o que ainda não foi provado;
9. como reproduzir;
10. como fazer rollback.

## Cabeçalho mínimo de um módulo

Cada README/contrato técnico deve conter:

```text
Purpose
Scope
Authority / provenance
Implementation paths
Runtime class
Dependencies
Inputs / outputs
Invariants
Build
Tests
Evidence / receipts
Failure modes
Security boundary
Performance boundary
Known gaps
Roadmap
Rollback
Version / release state
```

## Estados permitidos

Não inventar sinônimos quando estes bastarem:

```text
DOCUMENTED
IMPLEMENTED_HOSTED
IMPLEMENTED_NO_LIBC_ADAPTER
IMPLEMENTED_FREESTANDING
FREESTANDING_PROVED
PROVIDER_BACKED
REFERENCE_ONLY
VERIFIED_LOCAL
VERIFIED_CI
VERIFIED_PHYSICAL
BLOCKED
TOKEN_VAZIO
DEPRECATED
SUPERSEDED
```

## Regras de claim

```text
source exists          != build passed
build passed           != executed
executed               != reproduced
reproduced locally     != independently reproduced
hash matched           != authenticated identity
smaller binary         != faster execution
cross compiled         != physically executed
comment changed        != binary changed
```

## Build documentation

Registrar:

- compiler + version;
- target triple;
- preprocessor definitions;
- optimization profile;
- ISA flags;
- linker + version;
- linker flags;
- entrypoint;
- static/dynamic;
- symbols forbidden;
- artifact size;
- artifact digest;
- map/section report quando relevante.

## Fixed-point documentation

Para Q8/Q16/Q32/Q42:

- storage width;
- fractional bits;
- physical/logical unit;
- range;
- rounding;
- overflow;
- saturation;
- conversion rules;
- error/tolerance;
- KATs.

## Performance

Nenhuma expressão “mais rápido”, “menor fricção”, “zero overhead” ou equivalente sem comparator.

Receipt mínimo:

```text
before/after commit
before/after artifact digest
compiler/linker
flags
workload
iterations
warm-up
measurement source
binary size
latency/throughput if measured
correctness gate
```

## Release notes

Cada release note deve separar:

- Added;
- Changed;
- Fixed;
- Security;
- Compatibility;
- Evidence;
- Known gaps;
- Rollback;
- F_next.

## Rollback

Documentação é append-only/superseding quando trata evidência histórica.

Mudança de código deve apontar commit predecessor e manter formato/versionamento quando a saída canônica mudar.

## Auditoria

A documentação falha o gate quando:
- afirma feature ausente;
- chama hosted de freestanding;
- chama checksum de autenticação;
- chama cross-compile de execução física;
- oculta provider;
- omite mudança incompatível;
- remove TOKEN_VAZIO sem evidência.
