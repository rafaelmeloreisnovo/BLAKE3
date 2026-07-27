<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Contrato matemático RMR PAI42 V1

## Entrada

A entrada é exatamente um vetor de 42 contadores de ciclo ATA OMEGA:

```text
C = [c0, c1, ..., c41]
```

O contrato aceita o cabeçalho ATA `V1_EXTENDED` e o fluxo histórico
`LEGACY_COMPACT`, mas exige 42 registros válidos antes da projeção.

## Normalização determinística

Com `m = min(C)`, `M = max(C)` e `Q = 65536`:

```text
ri = ((ci - m) * Q) / (M - m)
```

Quando `M = m`, todos os raios recebem `Q/2`. Isso mantém o estado constante no
centro radial, sem divisão por zero e sem inventar variação.

## Geometria circular

Para cada índice `i`:

```text
θi = 2πi / 42
xi = cos(θi) * ri
yi = sin(θi) * ri
```

O núcleo C usa uma tabela Q16 fixa de 42 direções; não chama `sin`, `cos`, heap ou
ponto flutuante. A ferramenta Python recalcula a mesma referência para auditoria.

## Métricas

```text
mean      = média(ri)
variance  = média((ri - mean)^2) / Q
symmetry  = Q - média(|ri - r(i+21 mod 42)|)
stability = Q - variance
score     = 0.55 * symmetry + 0.45 * stability
```

Todos os valores são limitados ao intervalo Q16 `[0, 65536]`.

## Estados epistemológicos

| Afirmação | Estado |
| --- | --- |
| Projeção de 42 registros em círculo | `VERIFIED_BY_CODE` após testes |
| Identificação semântica de objeto/classe | `TOKEN_VAZIO` |
| `HW_SIG64` como PUF/serial físico único | `NOT_CLAIMED` |
| Ligação criptográfica da geometria ao BLAKE3 | `TOKEN_VAZIO` |
| Superioridade de desempenho | `TOKEN_VAZIO` até benchmark reproduzível |

## Falsificadores

O contrato falha se ocorrer qualquer um destes estados:

- quantidade diferente de 42;
- magic, versão, tamanho ou sequência ATA inválidos;
- registro truncado;
- raio Q16 fora do intervalo;
- divergência entre a referência C e a referência Python para a mesma fixture.
