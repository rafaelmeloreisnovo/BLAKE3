<!--
Copyright (c) 2026 Rafael Melo Reis
License: RMR Module License (see ../LICENSE_RMR)
-->

# Modelo RMR de execução paralela assíncrona orientada por dependências

## 1. Escopo e fronteira

Este documento especifica uma arquitetura autoral externa da camada `rmr/`.
Ele não altera o algoritmo BLAKE3, os vetores oficiais, a função de compressão
ou o núcleo criptográfico localizado em `src/`, `c/` e `reference_impl/`.

O objetivo é registrar, sem extrapolação, um modelo de escalonamento para
árvores de hashing no qual tarefas independentes progridem assim que suas
dependências locais ficam prontas, sem uma barreira global por nível da árvore.

Estado deste documento: `ARQUITETURA_ESPECIFICADA`.

## 2. Correção terminológica

Três camadas de paralelismo não devem ser confundidas:

1. **AVX-512 / SIMD**: uma instrução opera sobre várias lanes em lockstep. As
   lanes não são tarefas independentes que terminam em tempos diferentes.
2. **Multicore / MIMD**: núcleos distintos executam fluxos de instrução
   distintos e podem concluir tarefas em tempos diferentes.
3. **TBB / runtime de tarefas**: distribui tarefas prontas entre workers,
   normalmente com filas locais e work stealing.

Portanto, o comportamento "quem termina primeiro continua trabalhando" é uma
propriedade do escalonador multicore orientado por tarefas, não do AVX-512 em
si. AVX-512 pode acelerar cada tarefa vetorizável; TBB ou outro runtime decide
qual worker executa cada tarefa.

## 3. Baseline fork-join existente

O caminho C/TBB existente usa `oneapi::tbb::parallel_invoke` para executar os
subconjuntos esquerdo e direito da árvore. A chamada retorna somente depois que
os dois ramos concluírem.

Esse modelo é corretamente descrito como **fork-join**:

```text
             fork
          /        \
      esquerda    direita
          \        /
             join
```

O join local é semanticamente necessário para produzir o nó pai, porque o
parent CV depende dos CVs esquerdo e direito. Entretanto, um worker que termina
cedo não precisa permanecer ocioso: um runtime com work stealing pode fazê-lo
executar outra tarefa pronta enquanto o outro ramo ainda progride.

## 4. Modelo RMR: DAG assíncrono sem barreira global

A árvore é representada como um grafo acíclico dirigido de tarefas. Cada nó tem
um contador de dependências pendentes:

- folha pronta: `pending = 0`;
- pai de dois filhos: `pending = 2`;
- conclusão de um filho: grava seu CV no slot fixo e decrementa `pending`;
- transição `pending: 1 -> 0`: o pai entra na fila de tarefas prontas.

Não existe uma barreira que obrigue todos os nós do mesmo nível a terminar
antes que qualquer nó do nível seguinte comece.

```text
folha A concluída ----> publica CV esquerdo --+
                                             +--> pai fica READY quando pending=0
folha B concluída ----> publica CV direito ---+

worker que concluiu A:
  - se B já concluiu: pode executar o pai;
  - se B não concluiu: executa outra tarefa READY;
  - nunca precisa aguardar o nível inteiro da árvore.
```

A formulação correta da intenção autoral é:

> progresso local imediato, espera somente na dependência semântica mínima e
> redistribuição do worker enquanto a dependência ainda não estiver satisfeita.

## 5. Estado mínimo de uma tarefa

Uma implementação freestanding e sem alocação dinâmica pode usar uma tabela
estática de tarefas:

```c
struct rmr_task {
  uint32_t parent_index;
  uint32_t left_index;
  uint32_t right_index;
  uint32_t output_offset;
  _Atomic uint32_t pending;
  uint8_t kind;
  uint8_t state;
};
```

Estados permitidos:

```text
EMPTY -> READY -> RUNNING -> PUBLISHED -> COMPLETE
```

Transições inválidas devem falhar de modo auditável. Uma tarefa nunca pode ser
executada duas vezes, publicar no slot incorreto ou tornar o pai pronto antes
de ambos os filhos estarem publicados.

## 6. Fila de trabalho e distribuição entre cores

O modelo recomendado é:

- uma deque limitada por worker;
- push/pop local no caminho quente;
- work stealing pela extremidade oposta quando a fila local estiver vazia;
- fila global somente para bootstrap, overflow controlado ou recuperação;
- memória pré-alocada;
- ausência de `malloc` no caminho quente;
- padding por cache line para reduzir false sharing;
- contadores atômicos com ordem de memória mínima e explicitamente justificada.

Política de execução:

```text
while houver trabalho global:
    task = pop_local()
    if task inexistente:
        task = steal()
    if task inexistente:
        backoff_curto_ou_yield()
    else:
        executar(task)
        publicar_resultado(task)
        promover_dependentes_prontos(task)
```

A estratégia evita que um core fique parado apenas porque o ramo emparelhado
ainda não terminou. O core passa a colaborar com outro ramo pronto.

## 7. Determinismo criptográfico

Execução fora de ordem não pode alterar a ordem lógica da árvore.

Invariantes obrigatórios:

1. cada folha recebe `chunk_counter` determinístico;
2. cada pai possui slots fixos `left` e `right`;
3. a compressão do pai usa sempre `left || right`, nunca ordem de chegada;
4. cada CV é publicado uma única vez;
5. o resultado final deve ser bit a bit idêntico ao upstream oficial;
6. falha, cancelamento ou overflow nunca produzem hash parcial como válido.

Assim, a ordem de escalonamento pode variar, mas a redução criptográfica
permanece canônica.

## 8. Barreiras locais versus barreira global

O modelo não é "sem espera" em sentido absoluto. O nó pai precisa dos dois
filhos. A inovação operacional está em remover espera desnecessária:

| Sincronização | Estado |
| --- | --- |
| Join entre os dois filhos que formam um pai | `NECESSARIO` |
| Barreira entre todos os nós do mesmo nível | `EVITAVEL` |
| Worker ocioso enquanto existem outras tarefas prontas | `EVITAVEL` |
| Ordenação fixa esquerda/direita na compressão | `OBRIGATORIA` |

Nome técnico recomendado:

`dependency-driven asynchronous tree scheduler with local joins`

ou, em português:

`escalonador assíncrono de árvore orientado por dependências com joins locais`.

## 9. Relação com TBB

TBB pode implementar parte desse comportamento por meio de tasks, arenas e
work stealing. Entretanto, usar `parallel_invoke` recursivo não prova, por si
só, que a execução possui o melhor grafo, granularidade, afinidade, NUMA ou
custo de sincronização para esta carga.

Comparações futuras devem separar:

- upstream serial;
- upstream TBB fork-join;
- RMR assíncrono sem barreira global;
- cada variante com AVX2;
- cada variante com AVX-512 quando disponível.

TBB já existir no upstream não elimina a autoria de um grafo, política de
prontidão, memória estática, telemetria ou heurística RMR distinta.

## 10. Binário único, LTO e especialização

Transformar componentes em um único binário pode permitir:

- inlining interprocedural;
- eliminação de código morto;
- internalização e redução de símbolos;
- constant propagation entre módulos;
- melhor especialização por arquitetura;
- redução de fronteiras ABI no caminho quente.

Isso não garante ganho automático. Um binário monolítico também pode:

- aumentar pressão de instruction cache;
- dificultar dispatch portátil;
- aumentar tempo de compilação;
- ampliar impacto de uma regressão;
- impedir atualização isolada de backends;
- produzir pior código quando LTO ou PGO estiverem mal configurados.

A regra correta é:

`monolítico` = hipótese de otimização, não prova de superioridade.

A decisão deve ser sustentada por diff de assembly, tamanho de texto, símbolos,
IPC, cache misses, branch misses, energia e throughput.

## 11. Métricas obrigatórias

Uma implementação não deve ser promovida apenas por throughput médio.
Registrar:

- hashes/s e GiB/s;
- ciclos por byte e ciclos por hash;
- mediana, MAD, desvio padrão e CV;
- p50, p95, p99 e máximo;
- escalabilidade de 1, 2, 4, 8, 16, 32 e 64 workers;
- eficiência paralela `speedup / workers`;
- steals tentados, bem-sucedidos e falhos;
- tempo ocioso por worker;
- quantidade e duração dos joins locais;
- LLC misses, TLB misses, branch misses e IPC;
- frequência efetiva e throttling sob AVX-512;
- energia por hash quando RAPL ou métrica equivalente existir;
- identidade bit a bit contra vetores oficiais.

## 12. Estados de evidência

| Alegação | Estado atual |
| --- | --- |
| O modelo descrito evita barreira global por nível | `ARQUITETURA_ESPECIFICADA` |
| O pai continua exigindo os dois filhos | `INVARIANTE_LOGICA` |
| Worker livre pode executar outra tarefa pronta | `PROPRIEDADE_DO_MODELO` |
| TBB pode oferecer work stealing | `CAPACIDADE_DO_RUNTIME` |
| O fork atual já implementa integralmente este DAG | `TOKEN_VAZIO` |
| O modelo supera `parallel_invoke` em 64 cores | `TOKEN_VAZIO` |
| AVX-512 amplia a vantagem medida | `TOKEN_VAZIO` |
| Um binário único é mais rápido | `TOKEN_VAZIO` |

`TOKEN_VAZIO` significa que a evidência necessária ainda não foi produzida; não
significa falha, impossibilidade ou conclusão negativa.

## 13. Gates de implementação

Antes de integrar código ao caminho de produção:

1. `G0_PROVENIENCIA`: permanecer dentro de `rmr/`;
2. `G1_CORRECAO`: vetores oficiais bit a bit;
3. `G2_RACE_FREE`: ThreadSanitizer ou verificação equivalente quando possível;
4. `G3_BOUNDED_MEMORY`: filas e tabela sem overflow silencioso;
5. `G4_DETERMINISMO`: mesmo hash em múltiplas ordens de execução;
6. `G5_BASELINE`: comparar contra serial e TBB upstream fixados por SHA;
7. `G6_ESCALA`: medir workers e NUMA;
8. `G7_ENERGIA`: registrar frequência/potência quando o hardware permitir;
9. `G8_ROLLBACK`: fallback para o caminho oficial em qualquer falha.

## 14. Próxima implementação verificável

A sequência de menor retrabalho é:

1. especificar o grafo e os invariantes — **este documento**;
2. criar protótipo isolado em `rmr/`, sem alterar o core;
3. executar em shadow mode e comparar bit a bit;
4. medir em hardware disponível;
5. somente depois habilitar TBB/AVX-512/NUMA em servidor adequado;
6. promover alegações de `TOKEN_VAZIO` apenas quando os artefatos brutos forem
   publicados.

## 15. Síntese

O modelo não pede que todos esperem todos. Ele exige apenas que cada pai espere
os dois filhos que realmente formam aquele pai. Todo worker liberado continua
processando outra tarefa pronta. Essa diferença separa uma árvore com barreira
global de um grafo assíncrono orientado por dependências, preservando a ordem
criptográfica e aumentando a oportunidade de ocupação dos cores.
