<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Implementation Status V1

**Snapshot de código:** `29e4525e5b1e6dfd79167586da8d19722c3cc65b`  
**Data:** 2026-09-26  
**Regra:** status vem do código e da evidência observável, não do nome do arquivo.

## Estados normativos

- `DOCUMENTED`: contrato existe, execução não demonstrada.
- `IMPLEMENTED_HOSTED`: código existe e depende de runtime/OS/libc.
- `IMPLEMENTED_FREESTANDING`: core não depende de libc/OS, mas o artefato final ainda precisa de prova.
- `FREESTANDING_PROVED`: link/auditoria demonstram ausência de runtime dinâmico e símbolos proibidos no escopo.
- `PROVIDER_BACKED`: implementação depende de provider externo explícito.
- `REFERENCE_ONLY`: catálogo/referência, sem incorporação automática.
- `VERIFIED_LOCAL`: receipt local existe.
- `TOKEN_VAZIO`: evidência/implementação necessária não existe ou não foi localizada.
- `BLOCKED`: promoção impedida por gate conhecido.

## Matriz code-first

| Componente | Código observado | Estado | Limite |
| --- | --- | --- | --- |
| PAI CLI | `rmr/core/main.c`, `cli.c` | IMPLEMENTED_HOSTED | stdio/string e comandos hosted |
| PAI scan | `rmr/core/scan.c` | IMPLEMENTED_HOSTED | dirent/stat/FILE/realloc/qsort |
| PAI sign | `rmr/core/sign.c` | IMPLEMENTED_HOSTED | FILE/getenv/time/gmtime; “signature” histórica não é assinatura PKI |
| SHA-256 core | `rmr/core/hash_sha256.c` | MIXED_CORE_AND_HOSTED_IO | algoritmo em memória + FILE no mesmo TU |
| BLAKE3 adapter | `rmr/core/hash_blake3.c` | IMPLEMENTED_HOSTED_ADAPTER | bytes usam API upstream; arquivo usa FILE |
| Crypto runtime | `rmr/crypto/runtime/` | PROVIDER_BACKED | OpenSSL 3 para várias primitivas |
| Forensic time | `rmr/crypto/tools/forensic_time_attest.py` | IMPLEMENTED_HOSTED_SIDECAR | stdlib Python; assinatura/TSA externos permanecem separados |
| PAI42 C | `rmr/pai42/src/` | IMPLEMENTED_FIXED_Q16_CORE | bridge Python fora do hot path |
| FIXED256 ASM | `rmr/fixed256/asm/` | IMPLEMENTED_BARE_METAL | primitiva de transporte/redução, não criptografia |
| Custody16 core | `rmr/freestanding_custody16/include,src` | IMPLEMENTED_FREESTANDING | filesystem/clock ficam fora |
| Custody16 cross-link | `build/build_cross_matrix.sh` + receipt histórico | FREESTANDING_PROVED_LOCAL | x86_64/AArch64/ARMv7 no escopo do receipt |
| Custody16 Termux adapter | `adapters/termux_linux/` | IMPLEMENTED_NO_LIBC_OS_ADAPTER | usa syscalls; não é bare metal |
| Topology auditor Python | `rmr/tools/rmr_topology_audit.py` | IMPLEMENTED_HOSTED_TOOL | ferramentas locais opcionais |
| SIMPERF | `rmr/benchmark_framework/simperf/` | ANALOGY_ESTIMATE | não substitui benchmark físico |

## Contradições corrigidas documentalmente

### “PAI — C bare metal”
O CLI atual usa libc/POSIX. Portanto o rótulo histórico “C bare metal” não descreve o artefato PAI completo.

Classificação correta:

```text
PAI = HOSTED ORCHESTRATOR
Custody16/FIXED256 = FREESTANDING/BARE-METAL CORES
platform adapters = explicit boundary
```

### “merkle_root.txt”
A documentação code-first já registra que o arquivo histórico é uma **raiz linear de manifesto**, não uma árvore Merkle binária completa.

### “sign”
`pai sign` produz um registro determinístico/histórico com hash do binário e raiz do scan. Não promover esse arquivo para assinatura digital assimétrica ou certificado.

## Dependências que impedem “full standalone” no PAI atual

- `stdio.h`, `FILE`, `fopen/fread/fprintf`;
- `dirent.h`, `stat/lstat`;
- `malloc/realloc/free/strdup/qsort`;
- `getenv/time/gmtime`;
- CMake/host tools no processo de build;
- OpenSSL em `rmr/crypto/runtime` quando provider-backed.

Essas dependências são aceitáveis apenas em superfícies classificadas como hosted/provider.

## Gate para migração

Não substituir tudo de uma vez.

Para cada componente:

```text
1 PURE FUNCTION
2 FIXED MEMORY CONTRACT
3 NO-LIBC TYPES
4 PLATFORM ADAPTER
5 FREESTANDING LINK PROBE
6 SYMBOL AUDIT
7 KAT/NEGATIVE TEST
8 CROSS-ARCH RECEIPT
9 PROMOTION
```


## Delta 2026-09-26 — extraction cycle

Novas fronteiras materializadas:

| Superfície | Estado atual |
| --- | --- |
| `rmr/standalone/` source | IMPLEMENTED_FREESTANDING |
| standalone CI em `f6c6621b...` | VERIFIED_CI para os blobs daquela execução |
| build receipt extension posterior | PENDING_CURRENT_HEAD_CI |
| SHA-256 memory/file split | IMPLEMENTED_PENDING_CURRENT_HEAD_CI |
| BLAKE3 bytes/file split | IMPLEMENTED_PENDING_CURRENT_HEAD_CI |
| prime neighbor extraction | PURE_INTEGER_CORE_SOURCE_PENDING_CI |
| validate integer gate extraction | PURE_INTEGER_GATE_SOURCE_PENDING_CI |

### Receipt standalone já observado

No commit `f6c6621b5b6664f38a479a161a3bd380f3fc9068`:

```text
host_selftest = PASS
x86_64 FREESTANDING_ARTIFACT = PASS
AArch64 FREESTANDING_ARTIFACT = PASS
ARMv7 FREESTANDING_ARTIFACT = PASS
```

Toolchain observado no CI:

```text
Clang 18.1.3
LLD 18.1.3
GNU readelf 2.42
```

O build script foi posteriormente ampliado para emitir tamanho/hash/símbolos; essa versão precisa de novo receipt antes de promoção.
