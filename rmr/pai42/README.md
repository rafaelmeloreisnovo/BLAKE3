<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR PAI42 — ponte geométrica para ATA OMEGA

`rmr/pai42/` formaliza dentro do Blacktrain/RMR a ligação que antes estava
espalhada entre o `PAI.py` matemático e o binário `ATA_OMEGA.bin`.

A pasta não modifica o algoritmo BLAKE3 e não coloca Python no hot path. Ela
transforma os 42 registros temporais do ATA em uma observação circular fixa,
auditável e independente de bibliotecas externas.

## Fluxo

```text
HWIF AArch64/x86_64
        ↓
ATA_OMEGA.bin (V1 ou compacto)
        ↓
42 ciclos validados
        ↓
normalização Q16
        ↓
42 direções circulares
        ↓
raio + x/y + variância + simetria + estabilidade
        ↓
relatório de evidência
```

## Estrutura

| Caminho | Papel |
| --- | --- |
| `CONTRACT.md` | fórmula, invariantes, claims e falsificadores |
| `include/rmr_pai42.h` | API fixa, sem heap |
| `src/rmr_pai42.c` | núcleo Q16 sem ponto flutuante |
| `tools/pai42_bridge.py` | leitor ATA e relatório JSON `stdlib-only` |
| `schema/pai42-observation.schema.json` | contrato de intercâmbio do relatório |
| `tests/rmr_pai42_selftest.c` | teste do núcleo C |
| `tests/test_pai42_bridge.py` | V1, compacto e truncamento |
| `tests/run_tests.sh` | execução local dos dois testes |

## Fronteira com o PAI.py histórico

O arquivo histórico reunia formas, Pitágoras, Fibonacci/Tribonacci, Mandelbrot,
rotação, 42 amostras de circunferência e escolha por
`0.55 × simetria + 0.45 × estabilidade`.

Este módulo extrai apenas o invariante necessário para o RMR:

```text
42 observações → círculo → medida determinística
```

Mandelbrot, imagens, NumPy, Pillow e Matplotlib não entram nesta camada. Eles
podem consumir o JSON depois, fora do caminho de captura e custódia.

## Uso

```sh
python3 rmr/pai42/tools/pai42_bridge.py ATA_OMEGA.bin \
  --out PAI42_OBSERVATION.json
```

Testes:

```sh
sh rmr/pai42/tests/run_tests.sh
```

## Garantias atuais

- tamanho fixo de 42 pontos;
- suporte ao ATA V1 e ao formato compacto histórico;
- normalização inteira Q16;
- tabela circular fixa no C;
- nenhuma alocação dinâmica no núcleo;
- nenhuma alteração em `src/`, `c/`, `reference_impl/` ou no hash BLAKE3.

## Limites

- o resultado é uma projeção geométrica de ciclos, não reconhecimento universal;
- `HW_SIG64` continua sendo fingerprint observável, não segredo nem PUF provada;
- o JSON não é assinatura digital;
- integração com `pai` CLI, SIMD/NEON e hashing da observação permanecem
  `TOKEN_VAZIO` até implementação, teste e benchmark próprios.
