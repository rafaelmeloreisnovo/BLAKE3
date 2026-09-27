<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Roadmap V1 — Hosted to Standalone

## Princípio

Migrar **por capacidade**, não por reescrita total.

## R0 — inventário e verdade documental

Estado: **IN_PROGRESS**

- classificar cada módulo como hosted/freestanding/provider/reference;
- corrigir nomes históricos que excedem o código;
- centralizar START HERE;
- criar status matrix, build contract, release notes;
- manter proveniência upstream/RMR.

Gate de saída: documentação não contradiz imports, linker ou receipts.

## R1 — standalone foundation

Estado: **IMPLEMENTED / CI PASS HISTÓRICO; CURRENT-HEAD RECEIPT PENDING**

- tipos compiler-native;
- memory primitives próprias;
- contrato Q8/Q16/Q32/Q42;
- macros de atributos/branch hints centralizadas;
- probe `-nostdlib`;
- cross-link x86_64/AArch64/ARMv7;
- symbol audit.

Gate: `PT_INTERP=0 && DT_NEEDED=0 && unexpected_UND=0`.

## R2 — separar hashing bytes vs filesystem

Estado: **IMPLEMENTED / CURRENT-HEAD CI PENDING**

- SHA-256 in-memory em TU puro;
- file reader como adapter;
- BLAKE3 bytes adapter separado de file adapter;
- KAT idêntico antes/depois.

Gate: mesmos digests para fixtures; pure TU sem stdio.

## R3 — scan/custody core

Estado: **PARTIAL via Custody16**

- adapter enumera;
- core recebe path/size/bytes;
- ordenação/canonicalização versionada;
- buffers fixos;
- duas varreduras quando aplicável;
- sem heap no core.

Gate: fixture canônica reproduzida nas arquiteturas suportadas.

## R4 — sign/authority split

Estado: **PLANNED**

Separar:

```text
digest record
!= MAC
!= digital signature
!= certificate identity
!= trusted timestamp
```

Pure core prepara preimage; provider/secure element/PKI fica no adapter autorizado.

## R5 — Q profile convergence

Estado: **PLANNED**

Para cada módulo numérico:
- medir faixa;
- erro máximo;
- overflow;
- saturação;
- custo;
- Q profile.

Não migrar float para Q apenas por estética.

## R6 — linker specialization

Estado: **PLANNED**

- GC de seções;
- ICF seguro;
- LTO opcional;
- entrypoint por ISA;
- linker map;
- comparar tamanho e símbolos;
- nenhuma remoção baseada apenas em “parece redundante”.

## R7 — cross-architecture physical receipts

Estado: **PARTIAL**

- x86_64;
- ARMv7;
- AArch64;
- depois WASM/RISC-V/PPC onde houver toolchain/runner real.

Cross-compilar != executar fisicamente.

## R8 — evidence envelope

Estado: **PARTIAL**

Unificar:

```text
source
artifact
build flags
binary map
execution
receipt
parent
timestamp sidecar
signature reference
rollback pointer
```

## F_next

Implementar R1 em módulo isolado, sem alterar o PAI hosted; depois usar R1 como dependência interna de novos pure cores.


## R2.5 — pure integer extraction

Estado: **IMPLEMENTED / CI PENDING**

- `base_prime.c`: prev/next prime sem I/O/heap;
- `validate_core.c`: gate inteiro sem float/libm;
- adapters hosted preservam parsing/relatório.

Gate: object symbol audit sem undefined inesperado.
