# RAFAELIA · Matriz de prioridade vetorial para BLAKE3

Objetivo: separar documentação, código, testes, benchmarks e hipóteses para que o repositório avance por prova, não por ruído.

## Vetor de avaliação

```text
v = (intencao, observacao, ruido, transmutacao, memoria, coerencia, urgencia, latencia, evidencia, obviedade)
```

## Fórmula de valor

```text
valor = 0.20*evidencia + 0.18*urgencia + 0.16*memoria + 0.14*transmutacao + 0.12*latencia + 0.08*obviedade + 0.06*intencao + 0.04*ruido + 0.02*coerencia
```

## Prioridades em cripto/benchmark

| Camada | Aplicação | Critério de prova |
|---|---|---|
| Evidência | KATs, hashes, vetores oficiais, comparação byte-a-byte | saída igual ao vetor esperado |
| Urgência | qualquer falha em KAT, endianess, carry, length handling | teste falho reproduzido e corrigido |
| Obviedade | comando único de build/test/bench | `make test` ou script equivalente documentado |
| Latência | otimizações SIMD/NEON, ARM32, Android/Termux | benchmark antes/depois com ambiente descrito |
| Ruído fértil | divergência pequena de digest/performance | issue técnica com input mínimo |

## Estados permitidos

```text
[PROVADO]      passa KAT/referência
[PARCIAL]      compila mas falta vetor completo
[HIPOTESE]     otimização ou desenho sem medição
[REGRESSAO]    quebrou comportamento antes válido
[URGENTE]      risco criptográfico/correção obrigatória
```

## Blending correto

| Tipo | Tratamento |
|---|---|
| código | revisar segurança, UB, overflow, endianess |
| benchmark | registrar CPU, flags, compilador, tamanho de input |
| documentação | distinguir especificação oficial, implementação e extensão RAFAELIA |
| otimização | medir antes/depois; nunca trocar correção por velocidade |
| KAT | prioridade máxima sobre estética ou narrativa |

## Próximo ciclo sugerido

1. Inventariar arquivos de teste e benchmark.
2. Criar comando canônico de validação.
3. Separar README técnico de notas exploratórias.
4. Registrar resultados com data, commit, compilador e arquitetura.
5. Abrir/fechar gaps por evidência.

## Retroalimentar[3]

- F_ok: a cripto passa a ser guiada por KAT/evidência.
- F_gap: falta preencher com arquivos reais do repo.
- F_next: conectar matriz a scripts de teste e benchmarks reprodutíveis.
