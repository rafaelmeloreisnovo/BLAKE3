<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR — START HERE

**Documento canônico de entrada operacional**  
**Estado:** ACTIVE / CODE-FIRST / EVIDENCE-FIRST  
**Escopo:** somente a camada autoral `rmr/`. O núcleo BLAKE3 upstream permanece separado.

## 1. Regra de leitura

Use esta ordem:

```text
START HERE
  -> IMPLEMENTATION STATUS
  -> ARCHITECTURE
  -> BUILD/BINARY CONTRACT
  -> PROVENIENCE
  -> EVIDENCE
  -> ROADMAP
  -> RELEASE NOTES
```

Não promover narrativa acima do código.

```text
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
```

## 2. Superfícies reais

| Superfície | Estado observado | Dependência/runtime |
| --- | --- | --- |
| `rmr/core/` + `rmr/ui/` (PAI) | IMPLEMENTED_HOSTED | libc/POSIX em várias rotas |
| `rmr/core/hash_blake3.c` | IMPLEMENTED_HOSTED_ADAPTER | API BLAKE3 upstream + FILE para caminho de arquivo |
| `rmr/crypto/runtime/` | IMPLEMENTED_PROVIDER_BACKED | OpenSSL 3 quando disponível |
| `rmr/pai42/src/` | IMPLEMENTED_FIXED_Q16_CORE | C de tamanho fixo; bridge Python é hosted |
| `rmr/fixed256/asm/` | IMPLEMENTED_BARE_METAL | sem libc/runtime no kernel ASM |
| `rmr/freestanding_custody16/` | IMPLEMENTED_FREESTANDING | core sem libc/heap; adapter de filesystem é separado |
| `rmr/crypto/tools/forensic_time_attest.py` | IMPLEMENTED_HOSTED_SIDECAR | Python stdlib; não está no core freestanding |

## 3. O que “standalone/freestanding” significa neste repositório

Uma superfície só recebe estado `FREESTANDING_PROVED` quando o artefato final satisfaz, no mínimo:

- compilação com `-ffreestanding -fno-builtin`;
- link com `-nostdlib`;
- ausência de `PT_INTERP`;
- ausência de `DT_NEEDED`;
- ausência de símbolos indefinidos inesperados;
- ausência de heap/runtime externo no core;
- entrypoint explícito;
- receipt reproduzível da verificação.

Filesystem, relógio, rede, terminal e processo são responsabilidades de **adapter**, não do pure core.

## 4. Comentários e binário

Comentários C/C++ normais são removidos pelo pré-processador e **não devem ser usados como mecanismo de otimização de binário**.

O que efetivamente pode afetar código/binário:

- `#define`, `#if`, `#pragma`;
- atributos de função/dado;
- `inline` / `noinline`;
- visibilidade e seções;
- `-O2/-O3/-Os/-Oz`;
- `-ffunction-sections -fdata-sections`;
- linker GC/ICF;
- LTO;
- alinhamento;
- seleção de ISA;
- linker script;
- símbolos realmente referenciados.

Comentários continuam obrigatórios onde documentam contrato, fronteira, proveniência, pressuposto ou risco.

## 5. Política de fixed-point

Q8, Q16, Q32 e Q42 são **perfis numéricos**, não níveis de segurança nem de otimização.

Cada módulo deve declarar:

```text
storage_width
fraction_bits
rounding
saturation/overflow
units
domain
error_bound
```

Sem esses campos, o perfil numérico é `TOKEN_VAZIO_CONTRACT`.

## 6. Documentos normativos

- `rmr/docs/RMR_IMPLEMENTATION_STATUS_V1.md`
- `rmr/docs/RMR_STANDALONE_ARCHITECTURE_V1.md`
- `rmr/docs/RMR_BUILD_AND_BINARY_CONTRACT_V1.md`
- `rmr/docs/RMR_ROADMAP_V1.md`
- `rmr/docs/RELEASE_NOTES_RMR_2026-09-26.md`
- `rmr/PUBLIC_PRIVATE_BOUNDARY.md`
- `rmr/PROVENIENCE.md`
- `rmr/crypto/CUSTODY.md`
- `rmr/docs/HOTPATH_CONTRACT.md`

## 7. Gate de documentação

Uma feature só pode ser descrita como implementada quando existir caminho de código correspondente.

Uma feature só pode ser descrita como executada quando houver receipt.

Uma feature só pode ser descrita como freestanding quando houver prova do artefato final.

Uma feature só pode ser descrita como reproduzida independentemente quando houver evidência externa ao autor/ambiente original.

## 8. R3

```text
F_ok   = fronteiras hosted/freestanding/provider/reference agora explícitas
F_gap  = PAI hosted ainda mistura lógica pura com filesystem/tempo/UI
F_next = extrair pure cores por módulos, manter adapters separados e provar cada artefato no linker
```
