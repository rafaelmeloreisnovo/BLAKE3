<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Friction Measurement Protocol V1

## Objetivo

Transformar “fricção” em métricas observáveis.

Não existe uma única métrica universal de fricção.

## Vetor mínimo

[
F = (
bytes,
text,
data,
bss,
symbols,
undefined,
dynamic_deps,
branches,
loops,
allocations,
syscalls,
latency,
throughput,
energy
)
]

Nem todos os campos estarão disponíveis em todo ambiente. Ausente = `TOKEN_VAZIO`.

## Build receipt mínimo

Para cada before/after:

```text
source_commit
toolchain
target_triple
CFLAGS
LDFLAGS
artifact_sha256
text_bytes
data_bytes
bss_bytes
defined_symbol_rows
undefined_symbols
PT_INTERP
DT_NEEDED
correctness_gate
```

## Performance

Tamanho menor não implica execução mais rápida.

```text
binary_size_delta != latency_delta
symbol_delta      != energy_delta
branch_delta      != semantic_improvement
```

Performance requer workload e medição física/reproduzível.

## Comentários

Normalmente comentários C/C++ desaparecem no preprocessamento e não alteram o código gerado.

Exceções indiretas que podem mudar o artefato ou metadados:

- build com debug/line tables;
- uso de `__LINE__` ou geração dependente de posição;
- ferramentas que embutem source;
- preprocessadores/geradores customizados;
- mudanças junto com diretivas/macros.

Portanto, se uma alteração apenas de comentário mudar o binário de release, isso deve ser tratado como **evento a investigar**, não como propriedade presumida.

## Flags

Cada otimização deve apontar o mecanismo efetivo:

- section GC;
- ICF;
- LTO;
- inline/noinline;
- visibility;
- ISA;
- fixed frame/tail elimination;
- fixed-point;
- linker script;
- alignment.

## Q profiles

Q8/Q16/Q32/Q42 entram no vetor de fricção somente após registrar:
- range;
- error bound;
- saturation;
- conversion cost;
- generated helpers;
- artifact size;
- benchmark.

Q42 não é automaticamente melhor que Q16.

## Promotion

Só dizer “reduziu fricção” quando a dimensão reduzida for nomeada.

Exemplo aceitável:

```text
text_bytes: 4120 -> 3688 (-10.5%)
correctness: PASS
target: armv7
toolchain: ...
```

Não aceitável:

```text
friction = zero
```

sem definição e evidência.
