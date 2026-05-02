<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see rmr/LICENSE_RMR)
-->

# HOTPATH_CONTRACT.md

## Escopo e objetivo

Este contrato define regras obrigatórias para **rotas críticas** de execução em
`rmr/`, com foco em previsibilidade de latência, determinismo e portabilidade
entre compiladores. O upstream BLAKE3 continua como referência absoluta do
núcleo criptográfico; este contrato governa apenas a camada autoral em `rmr/`.

## Camadas e invariantes

### 1) `rmr/hwif/asm/*`

Invariantes:
- Somente operações mínimas ABI-safe (prólogo/epílogo correto, preservação de
  registradores callee-saved conforme ABI da plataforma).
- Sem chamadas externas (`call`/`bl`) no hot path de backend.
- Sem alocação dinâmica, sem IO, sem acesso a relógio/sistema operacional.
- Sem dependência de estado global mutável.

### 2) `rmr/hwif/rmr_hwif.c`

Invariantes:
- Seleção de backend deve permanecer lock-free/atômica conforme desenho atual.
- Dispatcher deve conter apenas lógica de roteamento (detecção/seleção), sem
  transformação pesada de dados.
- Qualquer fallback deve preservar equivalência funcional com backend portátil.
- Proibido inserir logs/IO no caminho de seleção.

### 3) `rmr/core/*`

Invariantes:
- Evitar heap em laços críticos.
- Quando heap for inevitável, concentrar alocação/liberação fora do loop
  crítico e documentar justificativa no código.
- Evitar branches raros dentro de loop quente quando possível (prefira tratar
  erro antes/depois do loop).
- Sem IO no hot path.

## Permitido / proibido (C/ASM)

### Permitido
- Operações aritméticas/bitwise determinísticas.
- Uso explícito de atômicos já estabelecidos para seleção de backend.
- `inline`/`always_inline` (com parcimônia) em funções de fronteira quente.
- Prefetch/alinhamento quando mensurável e sem alterar semântica.

### Proibido
- IO em hot path (arquivo, socket, terminal, syslog, `printf`, etc.).
- Branching dependente de erro raro dentro de loop crítico.
- Chamada externa em `rmr/hwif/asm/*` no caminho quente.
- Alocação heap recorrente dentro de loops críticos.
- Introduzir locks/mutexes na seleção de backend.

## Portabilidade de compilador (GCC/Clang)

### Flags mínimas recomendadas
- `-O2` ou `-O3`
- `-fno-strict-aliasing` quando houver risco de aliasing em ponteiros legacy.
- `-fwrapv` somente se código depender explicitamente de overflow assinado
  definido (caso contrário, preferir tipos unsigned).
- `-Wall -Wextra` para capturar regressões estruturais cedo.

### Atributos e inlining
- Centralizar macros de atributo (ex.: `RMR_ALWAYS_INLINE`, `RMR_NOINLINE`) em
  headers de utilidade e usar de forma consistente entre GCC/Clang.
- Evitar cascata de `always_inline` que aumente pressão de registradores sem
  ganho mensurável.

### Alinhamento
- Estruturas e buffers quentes devem declarar alinhamento explícito quando
  necessário (ex.: `__attribute__((aligned(64)))` ou equivalente).
- Mudanças de alinhamento devem vir acompanhadas de benchmark antes/depois.

### Barreiras e atômicos
- Para seleção de backend, usar semântica atômica explícita
  (acquire/release/relaxed conforme necessidade documentada).
- Não substituir atômicos por variáveis comuns em caminhos concorrentes.
- Em ASM, preservar barreiras mínimas exigidas pela arquitetura para evitar
  reordenação observável.

## Governança e revisão automática

Este contrato deve ser validado em toda revisão por meio de:

1. `rmr/include/rmr_governance.h`
   - Fonte canônica de versão e referência do contrato.
2. `rmr/docs/REVIEW.md`
   - Checklist de auditoria com itens explícitos deste contrato.

### Gatilhos objetivos para reprovação de revisão

- Presença de IO em arquivos de hot path (`rmr/hwif/*`, `rmr/core/*`).
- `malloc/calloc/realloc/free` dentro de loops críticos sem justificativa
  explícita e sem concentração fora do loop.
- `call`/`bl` em assembly de backend quente.
- Introdução de lock/mutex no dispatcher de backend.

### Comandos sugeridos para validação rápida

```sh
rg -n "printf|fprintf|fopen|write\(|send\(|recv\(|socket\(" rmr/hwif rmr/core
rg -n "malloc\(|calloc\(|realloc\(|free\(" rmr/core
rg -n "\b(call|bl)\b" rmr/hwif/asm
```

> Observação: estes comandos são triagem inicial; revisão humana continua
> obrigatória para contexto e falsos positivos.
