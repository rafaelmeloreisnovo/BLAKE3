<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Standalone Architecture V1

## Objetivo

Transformar funcionalidades RMR em módulos portáveis **sem dependência de libc/heap/OS no pure core**, sem reescrever o BLAKE3 upstream e sem esconder dependências de adapter.

## Arquitetura alvo

```text
                 +----------------------+
                 |  ORCHESTRATOR/HOST   |
                 | CLI, filesystem, UI  |
                 +----------+-----------+
                            |
                    explicit adapter ABI
                            |
        +-------------------+-------------------+
        |                   |                   |
+-------v------+    +-------v------+    +-------v------+
| CUSTODY CORE |    | NUMERIC CORE |    | HASH ADAPTER |
| fixed memory |    | Q8/16/32/42  |    | bytes only   |
+-------+------+    +-------+------+    +-------+------+
        |                   |                   |
        +-------------------+-------------------+
                            |
                 compiler/linker contract
                            |
              static freestanding artifact
```

## Invariantes do pure core

- nenhuma chamada de filesystem;
- nenhum relógio;
- nenhuma rede;
- nenhum `malloc/calloc/realloc/free`;
- nenhum `printf/fprintf/snprintf`;
- nenhuma variável de ambiente;
- nenhuma thread obrigatória;
- buffers e capacidades fornecidos pelo chamador;
- tipos definidos por builtins do compilador quando possível;
- aritmética inteira/fixed-point com contrato explícito;
- erro retorna status numérico;
- nenhum fallback silencioso de ISA/semântica;
- nenhuma função “mágica” gerada por template.

## O papel de `void`

`void` deve ser usado quando semanticamente correto:

- `void *` para buffers sem tipo na fronteira de memória;
- `(void)x` para parâmetro deliberadamente ignorado;
- função `void` quando não existe valor de retorno semântico.

Não usar `void` para apagar tipo, contrato ou erro. Fronteira genérica != ausência semântica.

## Modularização

Cada módulo deve ser pequeno e especializado:

```text
types
memory
fixed_point
hash_bytes
custody
serialization
verification
platform_adapter
entrypoint
```

A orquestração acontece no build e no linker, não por dependência dinâmica escondida.

## Compilador e linker

Perfil base:

```text
-std=c11
-ffreestanding
-fno-builtin
-fno-stack-protector
-fno-unwind-tables
-fno-asynchronous-unwind-tables
-ffunction-sections
-fdata-sections
-fvisibility=hidden
-fno-ident
-nostdlib
-static
-Wl,--gc-sections
-Wl,--build-id=none
-Wl,--no-undefined
```

ICF/LTO são permitidos somente quando suportados e acompanhados de equivalência funcional e receipt.

## Comments vs directives

Comentários documentam:
- invariante;
- unidade;
- domínio;
- risco;
- proveniência;
- razão de uma flag;
- fronteira entre core e adapter.

Diretivas efetivas ficam em macros/flags/linker. Não afirmar redução de binário causada por comentário comum.

## Perfis Q

| Perfil | fraction bits | armazenamento mínimo recomendado |
| --- | ---: | ---: |
| Q8 | 8 | 16/32 bits conforme domínio |
| Q16 | 16 | 32 bits |
| Q32 | 32 | 64 bits |
| Q42 | 42 | 64 bits |

A escolha é por erro, faixa e custo. Não existe perfil universalmente superior.

## Migração do PAI

O PAI atual permanece hosted até que cada função seja extraída.

Exemplo para scan:

```text
HOSTED NOW:
opendir/readdir/stat/qsort/realloc
        |
        v
TARGET:
adapter enumerates canonical records
        |
        v
pure custody core(path,size,bytes)
        |
        v
fixed output buffer + digest
```

O core não “descobre diretórios”; ele processa bytes e metadados canonizados fornecidos por adapter.

## Não regressão

A migração não pode:
- alterar digest BLAKE3 upstream;
- apagar receipts antigos;
- reclassificar hosted como freestanding sem prova;
- substituir algoritmo com implementação autoral e manter o mesmo nome;
- alterar output canônico sem versionar formato;
- remover caminho de rollback.
