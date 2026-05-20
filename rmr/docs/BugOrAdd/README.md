<!--
Copyright (c) 2026 Rafael
License: RMR Module License (see ../../LICENSE_RMR)
-->

# BugOrAdd — Base Conceitual Remodelável

Este diretório é reservado para **conceitos**, **reinterpretações** e
**reestruturações documentais** que não alteram o núcleo criptográfico do
BLAKE3 upstream.

## Objetivo

Registrar um mecanismo de estudo que organiza conhecimento em camadas, com
foco em:

- mapeamento toroidal de estados (`\mathbb{T}^7`);
- dinâmica de coerência/entropia (`C_t`, `H_t`, `\alpha`);
- atratores finitos de interpretação (`|\mathcal{A}| = 42`);
- múltiplas linguagens e variação prosódica (som, acento, cadência);
- tradução sem perda total de estrutura semântica.

## Leitura operacional do modelo

1. **Entrada**: `x = (dados, entropia, hash, estado)`.
2. **Mapeamento**: `s = ToroidalMap(x)` com `s \in [0,1)^7`.
3. **Atualização**: filtros exponenciais para coerência e entropia
   (`\alpha = 0.25`).
4. **Acoplamento**: `\phi = (1-H)\cdot C`, usado como modulador de
   estabilidade semântica.
5. **Convergência**: evolução para um conjunto de atratores interpretativos.
6. **Validação multi-camada**: correlações espectrais (`R`, `R_L`) e
   coerência geométrica (`bits_geom = log2(M\times N)`).

## Escopo e limites

- Este conteúdo é **externo/autoral** (RMR).
- É permitido refazer, reinterpretar e remodelar os conceitos do diretório.
- Não é permitido alterar o núcleo BLAKE3 (`src/`, `c/`, `reference_impl/`).

## Estrutura recomendada

- `README.md`: visão absoluta e contratos.
- `models/`: formalizações matemáticas por versão.
- `lexicon/`: dicionários multi-idioma e observações de prosódia.
- `experiments/`: hipóteses e resultados reprodutíveis.
- `notes/`: decisões de redesign e trilha de aprendizado.

## Critérios de qualidade para novos documentos

- declarar hipótese, variáveis, invariantes e limites;
- separar claramente o que é metáfora do que é implementação executável;
- registrar impacto em tradução (PT/EN/ZH/JA/HE/ARC/EL);
- manter rastreabilidade temporal (data e revisão).

## Estado atual

Este diretório nasce como **fundação de documentação absoluta** para
consolidar conhecimento e reduzir incoerência entre conceitos matemáticos,
linguísticos e computacionais.
