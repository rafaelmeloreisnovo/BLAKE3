<!--
Copyright (c) 2026 Rafael
License: RMR Module License (see ../../../LICENSE_RMR)
-->

# Mapa de Sinal Multilíngue

## Objetivo

Registrar diferenças de entonação, acentuação e cadência entre idiomas
(PT, EN, ZH, JA, HE, ARC, EL) sem colapsar tudo em uma única métrica.

## Princípios

- Distâncias podem divergir por eixo: `d_theta(u,v) != d_gamma(u,v)`.
- Mesmo conteúdo lexical pode produzir assinaturas acústicas diferentes.
- Tradução é tratada como transformação com perda controlada, não equivalência
  perfeita.

## Métricas úteis

- Similaridade espectral por língua: `R_L`.
- Entropia operacional: `entropy_milli`.
- Coerência de link: `E_link = alpha sin(delta_theta)cos(delta_phi)`.
