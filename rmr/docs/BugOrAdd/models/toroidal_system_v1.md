<!--
Copyright (c) 2026 Rafael
License: RMR Module License (see ../../../LICENSE_RMR)
-->

# Modelo Toroidal v1

## Estado e espaço

- Espaço: `T^7 = (R/Z)^7`.
- Estado: `s = (u,v,psi,chi,rho,delta,sigma)`.
- Entrada: `x = (dados, entropia, hash, estado)`.
- Mapeamento: `s = ToroidalMap(x)` com `s in [0,1)^7`.

## Dinâmica

- `C_{t+1} = (1-alpha)C_t + alpha C_in`
- `H_{t+1} = (1-alpha)H_t + alpha H_in`
- `alpha = 0.25`
- `phi = (1-H)*C`

## Invariantes de leitura

- Atrator interpretativo: `lim t->inf s(t) in A`
- Cardinalidade-alvo: `|A| = 42`
- Capacidade geométrica: `bits_geom = log2(M*N)`

## Observação

Este arquivo descreve camada conceitual, não prova formal do núcleo BLAKE3.
