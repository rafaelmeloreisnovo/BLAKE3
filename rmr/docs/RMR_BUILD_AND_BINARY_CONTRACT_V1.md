<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Build and Binary Contract V1

## 1. Objetivo

Definir o que precisa ser observado no **artefato final**, e não apenas no source.

## 2. Perfis

### HOSTED
Pode usar libc/OS. Deve declarar dependências.

### NO_LIBC_ADAPTER
Pode usar syscalls/ABI de plataforma, mas não libc.

### FREESTANDING_CORE
Nenhuma dependência de OS/libc/heap. Entrada e saída por memória fornecida.

### BARE_METAL_ENTRY
Entrypoint próprio; nenhuma syscall obrigatória.

### PROVIDER_BACKED
Primitiva externa declarada, versionada e testada.

## 3. Gates binários

Para `FREESTANDING_PROVED`:

```text
PT_INTERP absent
DT_NEEDED absent
unexpected UND = 0
forbidden symbols = 0
entrypoint explicit
build-id policy satisfied
source/flags/toolchain recorded
KAT or deterministic selftest PASS
```

## 4. Símbolos proibidos no pure core

Baseline:

```text
malloc calloc realloc free
printf fprintf snprintf
fopen fread fwrite fclose
pthread_create
dlopen dlsym
getenv
time gmtime localtime
```

Uma palavra presente no source não é prova de símbolo no binário; o gate final usa ELF/symbol table.

## 5. Redução de fricção verificável

Técnicas permitidas:

- fixed-size kernels;
- eliminar tail por contrato quando o domínio permitir;
- section GC;
- ICF seguro;
- visibilidade hidden;
- funções especializadas;
- buffers caller-owned;
- sem heap;
- separar cold error path;
- evitar formatação textual no hot path;
- fixed-point quando o erro for aceitável;
- SIMD/ASM por ISA com KAT equivalente;
- LTO somente quando reproduzível.

Toda alegação de melhoria precisa de:

```text
before artifact hash
after artifact hash
size before/after
workload
compiler/linker versions
flags
correctness gate
performance receipt (se performance for alegada)
```

## 6. Shadowing

Compilar módulos novos com `-Wshadow`. Para promover a gate estrita, usar `-Werror` após limpar warnings reais.

“Sem shadows” significa ausência de shadowing lexical/simbólico detectável, não ausência de camadas arquiteturais.

## 7. Rollback

Cada mudança de build profile deve ser um commit coerente e reversível.

Nunca sobrescrever receipt histórico. Nova medição cria successor receipt.
