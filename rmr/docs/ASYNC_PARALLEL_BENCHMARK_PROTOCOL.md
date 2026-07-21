<!--
Copyright (c) 2026 Rafael Melo Reis
License: RMR Module License (see ../LICENSE_RMR)
-->

# Protocolo reproduzível de benchmark paralelo RMR

## 1. Objetivo

Este protocolo mede, sem extrapolação, a diferença entre:

1. caminho upstream serial;
2. caminho upstream TBB baseado em fork-join;
3. futuro escalonador RMR orientado por dependências e joins locais;
4. variantes AVX2 e AVX-512 quando o hardware suportar.

O protocolo não pressupõe vitória de qualquer variante. Ele preserva
`TOKEN_VAZIO` até que dados brutos reproduzíveis existam.

## 2. Artefatos fixados

Cada execução deve registrar:

- SHA do upstream;
- SHA do fork;
- SHA do protótipo RMR;
- compilador, versão e target triple;
- linker e versão;
- CMake/Ninja/Make e versões;
- TBB e versão;
- flags completas de compilação e linkedição;
- microcode, kernel e sistema operacional;
- topologia de CPU, sockets, NUMA, cache e memória;
- estado de SMT;
- governor, turbo e limites de potência;
- disponibilidade real de AVX2/AVX-512.

Sem esses campos, o resultado recebe estado `EVIDENCIA_INCOMPLETA`.

## 3. Matriz mínima

### 3.1 Workers

Executar com:

`1, 2, 4, 8, 16, 32, 64`

Limitar a matriz ao número de cores físicos disponíveis. Não representar
threads lógicas como cores físicos sem declarar SMT.

### 3.2 Tamanhos de entrada

Executar pelo menos:

- 1 KiB;
- 4 KiB;
- 16 KiB;
- 64 KiB;
- 256 KiB;
- 1 MiB;
- 4 MiB;
- 16 MiB;
- 64 MiB;
- 1 GiB quando memória e tempo permitirem.

### 3.3 Modos

| ID | Scheduler | SIMD | Observação |
| --- | --- | --- | --- |
| `S0` | serial upstream | dispatch padrão | baseline funcional |
| `S1` | TBB fork-join upstream | AVX2 | baseline multicore |
| `S2` | TBB fork-join upstream | AVX-512 | somente se suportado |
| `R1` | DAG RMR | AVX2 | joins locais |
| `R2` | DAG RMR | AVX-512 | somente se suportado |
| `R3` | DAG RMR | portable | separa scheduler de SIMD |

## 4. Controles de ambiente

Quando permitido pelo ambiente:

- fixar afinidade de processo e workers;
- impedir migração entre NUMA nodes;
- alocar memória no node correspondente;
- registrar e, quando possível, fixar frequência;
- separar resultados com turbo ligado/desligado;
- registrar temperatura inicial e final;
- estabilizar página e cache antes da medição;
- evitar processos concorrentes;
- executar em modo de energia conhecido;
- registrar huge pages quando usadas;
- não misturar máquina virtual, contêiner e bare metal na mesma campanha.

Se um controle não puder ser aplicado, registrar `NAO_CONTROLADO`, nunca omitir.

## 5. Aquecimento e amostragem

Por combinação de modo, workers e tamanho:

1. mínimo de 5 aquecimentos fora da amostra;
2. mínimo de 32 amostras válidas;
3. ordem de variantes randomizada por bloco para reduzir viés térmico;
4. intervalo curto entre blocos quando houver throttling;
5. pelo menos 3 execuções independentes do processo;
6. dados brutos preservados, sem substituir por apenas média.

Resultados com outlier devem ser apresentados com e sem filtro. O critério de
filtro deve ser definido antes da análise.

## 6. Métricas de desempenho

Registrar por amostra:

- tempo monotônico;
- ciclos;
- bytes processados;
- hashes processados;
- throughput em GiB/s;
- ciclos/byte;
- workers ativos;
- CPU e NUMA node de cada worker;
- frequência efetiva;
- temperatura quando disponível.

Calcular:

- mínimo;
- mediana;
- média;
- máximo;
- MAD;
- desvio padrão;
- CV;
- p50, p95 e p99;
- intervalo de confiança;
- speedup contra `S0`;
- eficiência paralela `speedup / workers`.

## 7. Métricas do scheduler

Para `S1`, `S2`, `R1`, `R2` e `R3`, registrar:

- tarefas criadas;
- tarefas executadas;
- profundidade máxima;
- joins locais;
- tempo acumulado esperando dependência;
- tempo ocioso por worker;
- steals tentados;
- steals bem-sucedidos;
- steals falhos;
- tamanho máximo e médio das filas;
- overflow ou fallback;
- quantidade de tarefas executadas pelo worker que concluiu cedo;
- false sharing detectado ou suspeito.

A hipótese RMR só recebe suporte quando a redução de espera/ociosidade aparece
nos contadores e não apenas no throughput final.

## 8. Métricas de hardware

Quando `perf`, RAPL, PCM ou equivalente estiver disponível:

- instructions;
- cycles;
- IPC;
- branches e branch-misses;
- cache-references e cache-misses;
- LLC loads e LLC load misses;
- dTLB/iTLB misses;
- context switches;
- migrations;
- page faults;
- joules de package e DRAM;
- frequência média sob AVX-512.

Ausência de hardware counters: `TOKEN_VAZIO`, não estimativa inventada.

## 9. Correção e segurança

Antes de aceitar qualquer amostra de desempenho:

1. validar vetores oficiais;
2. comparar saída bit a bit com o upstream;
3. testar entradas vazias, parciais e limites de chunk;
4. testar ordens diferentes de execução do DAG;
5. repetir com 1 worker para equivalência funcional;
6. falhar em overflow de fila, tabela ou contador;
7. registrar qualquer fallback.

Uma divergência de hash invalida a campanha de desempenho correspondente.

## 10. Testes estatísticos

Para comparar duas variantes:

- usar Welch quando distribuições aproximadamente contínuas justificarem;
- usar Mann-Whitney ou bootstrap quando a distribuição for assimétrica;
- reportar tamanho de efeito, não somente `p`;
- corrigir múltiplas comparações quando necessário;
- não chamar diferença inferior ao ruído de ganho operacional.

Critério mínimo sugerido para claim de desempenho:

- diferença estatisticamente sustentada;
- tamanho de efeito relevante;
- repetição em pelo menos 3 processos;
- ausência de regressão de correção;
- commits e ambiente fixados.

## 11. Claims permitidos

### 11.1 Medição local

> No hardware, commits, flags e metodologia registrados, a variante X apresentou
> Y% de diferença em throughput e Z% de diferença em CV/p99.

### 11.2 Não permitido sem evidência

- “sempre duas vezes mais rápido”;
- “mais seguro criptograficamente”;
- “economiza 50% de qualquer datacenter”;
- “AVX-512 necessariamente amplia a vantagem”;
- “TBB espera ocioso em todos os casos”;
- “binário único é sempre superior”.

## 12. Estrutura de saída

Cada campanha deve produzir:

```text
results/<campaign_id>/
  manifest.json
  environment.json
  commits.json
  flags.txt
  raw_samples.csv
  scheduler_events.csv
  hardware_counters.csv
  correctness.json
  summary.json
  report.md
  sha256sums.txt
```

O `manifest.json` deve conter um identificador imutável da campanha e os hashes
de todos os demais arquivos.

## 13. Cadeia de custódia

Estados recomendados:

| Estado | Significado |
| --- | --- |
| `PLANEJADO` | protocolo definido, não executado |
| `EXECUTADO_LOCAL` | execução concluída em ambiente local |
| `REPRODUZIDO` | repetido por ambiente independente |
| `MODELADO` | projeção derivada de medições |
| `TOKEN_VAZIO` | evidência necessária ainda ausente |
| `INVALIDADO` | correção ou metodologia falhou |

Nunca converter `MODELADO` em `MEDIDO` e nunca converter `TOKEN_VAZIO` em
conclusão negativa.

## 14. Gate de 64 cores / AVX-512 / TBB

A alegação de escalabilidade em servidor só pode avançar quando houver:

- hardware real identificado;
- 64 cores físicos ou descrição exata da topologia;
- pinning e NUMA registrados;
- AVX-512 confirmado em runtime;
- TBB e scheduler RMR comparados na mesma máquina;
- frequência e throttling registrados;
- dados brutos e hashes publicados;
- identidade bit a bit confirmada.

Até isso ocorrer:

`resultado_64c_avx512_tbb = TOKEN_VAZIO`

## 15. Síntese operacional

O protocolo foi construído para evitar retrabalho: mede separadamente algoritmo,
SIMD, scheduler, topologia e energia. Assim, um ganho não é atribuído ao AVX-512
quando veio do scheduler, nem ao scheduler quando veio de cache, afinidade ou
flags de compilação.
