<!--
Copyright (c) 2026 Rafael
License: RMR Module License (see ../../../LICENSE_RMR)
-->

# Protocolo de Experimentos v1

## Checklist mínimo

1. Definir hipótese objetiva.
2. Declarar variáveis (`C`, `H`, `alpha`, `R`, `R_L`).
3. Definir critério de parada e revisão.
4. Registrar seed, data e versão documental.

## Reprodutibilidade

- Cada execução deve gerar registro com timestamp.
- Resultados devem indicar claramente o que é metáfora e o que é medição.
- Nenhum experimento pode alterar pastas do núcleo (`src/`, `c/`,
  `reference_impl/`).
