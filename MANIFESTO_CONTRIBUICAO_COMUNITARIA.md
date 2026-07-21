<!--
Copyright (c) 2026 Rafael Melo Reis
License: RMR Module License (see rmr/LICENSE_RMR)
-->

# Manifesto mínimo de contribuição comunitária, desempenho e proveniência

## 1. Propósito

Este fork preserva integralmente a autoria e as licenças do BLAKE3 upstream e
registra separadamente as contribuições autorais de Rafael Melo Reis.

Este documento não atribui intenção sem prova e não acusa cópia por mera
semelhança. Ele registra três fatos distintos:

1. uma contribuição upstream ampla foi encerrada sem integração;
2. partes tecnicamente úteis nela contidas mereciam decomposição e revisão
   específica, ainda que o conjunto completo não fosse aceitável;
3. medições locais posteriores mostram vantagens relevantes de desempenho e
   previsibilidade operacional deste fork em regimes determinados.

A regra é simples:

`afirmação -> artefato -> método -> resultado -> limite da conclusão`

## 2. Registro da contribuição upstream

A pull request `BLAKE3-team/BLAKE3#533` foi aberta em 2025-11-25 e encerrada
sem merge no mesmo dia. Ela continha 24 commits, 10 arquivos alterados, 1.566
adições e 195 remoções.

O escopo incluía, entre outros pontos:

- aquecimento de cache em benchmarks;
- pré-alocação de buffers e saídas fora da região temporizada;
- uso de `black_box` para impedir eliminação indevida pelo compilador;
- observação explícita de variância, alinhamento e paralelismo SIMD;
- ajustes de lint, offsets, tipos e expressões modernas;
- documentação extensa sobre caminhos SSE2, SSE4.1, AVX2, AVX-512 e NEON.

O conjunto também continha excesso documental e mudanças que precisavam ser
separadas, corrigidas ou rejeitadas individualmente. Isso explica o custo de
revisão, mas não transforma cada mecanismo técnico apresentado em material
sem valor.

Uma resposta comunitária mais produtiva teria indicado quais commits eram:

- aproveitáveis;
- corrigíveis;
- redundantes;
- incompatíveis;
- ou inadequados ao upstream.

Encerrar o pacote inteiro não invalida automaticamente todas as ideias nele
contidas.

## 3. O que os benchmarks realmente medem

Os relatórios medem **throughput, ciclos, latência e variabilidade temporal**.
Eles não medem “acurácia”. Para uma função hash, correção significa produzir
saída bit a bit compatível com os vetores oficiais; porcentagens como 97% e 3%
no material referem-se a desempenho ou variância, não a acurácia criptográfica.

Também é necessário distinguir:

- **previsibilidade operacional**: menor CV, menor p95/p99 e menor dispersão;
- **correção criptográfica**: identidade bit a bit e aprovação dos vetores;
- **segurança criptográfica**: propriedade que não pode ser inferida de um
  benchmark de velocidade.

## 4. Campanha A — RDTSC, 3 aquecimentos e 15 ciclos

Ambiente registrado: Linux x86_64 em contêiner, GCC 13.x,
`-O3 -march=native -DNDEBUG`, 3 aquecimentos e 15 medições por tamanho.

| Entrada | Resultado observado do fork |
| --- | --- |
| 1 KiB | `+3,9%` throughput e pico máximo de ciclos `-26,9%` |
| 64 KiB | `+98,3%` throughput; mediana de ciclos `-49,5%`; máximo `-51,2%` |
| 1 MiB | `+10,4%` throughput; mediana `-9,3%`; ambos com ruído elevado |
| 16 MiB | `-3,5%` throughput e pico máximo pior; upstream venceu nesse ensaio |

No ponto de 64 KiB, o relatório registrou `7,12 GB/s` no fork contra
`3,59 GB/s` no upstream, aproximadamente duas vezes o throughput. A dispersão
mínimo-máximo registrada foi `130%` no fork contra `371%` no upstream.

Esse ensaio sustenta vantagem forte no regime de 64 KiB, mas não sustenta
superioridade universal.

Artefato:

- `BENCHMARK_RESULTS.txt`
- SHA-256: `edc85ae6b1a3e4a8026a59a22de02ba4a7a0723617be94a647e49eab72ed0032`

## 5. Campanha B — benchmark industrial, 5 aquecimentos e 32 amostras

Ambiente registrado: Linux x86_64 em contêiner, GCC 13.x,
`-O3 -march=native -DNDEBUG`, RDTSC sincronizado, 5 aquecimentos e 32 amostras
por tamanho.

| Entrada | Upstream | Fork | Leitura correta |
| --- | ---: | ---: | --- |
| 1 KiB | `0,05 GB/s`, CV `16,79%` | `0,04 GB/s`, CV `18,84%` | upstream significativamente mais rápido (`p=0,037`) |
| 64 KiB | `2,14 GB/s`, CV `21,85%` | `2,10 GB/s`, CV `21,77%` | diferença não significativa (`p=0,244`) |
| 1 MiB | `3,01 GB/s`, CV `282,10%` | `5,94 GB/s`, CV `14,75%` | fork `+97,0%` throughput e muito mais previsível |
| 16 MiB | `6,33 GB/s`, CV `8,33%` | `6,30 GB/s`, CV `3,08%` | throughput equivalente; fork com CV `-63%` |

### Resultado mais forte: 1 MiB

- throughput: `+97,0%`;
- latência média: `-49,4%`;
- ciclos médios: `-49,3%`;
- coeficiente de variação: `282,10% -> 14,75%`;
- redução relativa da variabilidade: `94,8%`;
- p99: `5,414 ms -> 0,280 ms`, aproximadamente `19,3x` menor;
- teste de Welch: `p < 0,001`.

A razão `282,10 / 14,75` é aproximadamente `19,1`. Portanto, neste ambiente e
neste tamanho, o fork foi cerca de 19 vezes mais estável pelo CV registrado.
Isso é evidência direta de previsibilidade temporal superior no regime
cache-bound medido.

### Resultado importante: 16 MiB

O throughput ficou praticamente empatado (`-0,5%` no fork), mas o CV caiu de
`8,33%` para `3,08%`. A vantagem aqui é estabilidade, não velocidade média.

Artefato:

- `BENCHMARK_INDUSTRIAL_REPORT-2.txt`
- SHA-256: `b29e3242e3ae6b617465397dd3a8558233838b73314c204af60dd809b8800508`

## 6. Síntese técnica permitida

Os dois ensaios independentes não colocaram o maior ganho no mesmo tamanho:

- campanha A: ganho máximo em 64 KiB;
- campanha B: ganho máximo em 1 MiB.

Isso não apaga o ganho; mostra sensibilidade a cache, ruído, afinidade,
ambiente e metodologia. A conclusão tecnicamente sustentável é:

> Neste ambiente x86_64 e nas campanhas registradas, o fork demonstrou
> vantagens fortes e reproduzíveis em regimes cache-bound, chegando a
> aproximadamente duas vezes o throughput, além de apresentar redução muito
> relevante de variância e latência de cauda em 1 MiB e 16 MiB.

Não é permitido converter essa conclusão em “sempre duas vezes mais rápido” ou
“mais seguro criptograficamente” sem novas evidências.

## 7. Warnings, compilador e linker

Warnings são diagnósticos; não são comandos executados pelo linker.

O ganho pode surgir quando esses diagnósticos orientam mudanças reais, como:

- remoção de aliasing desnecessário e uso correto de `restrict`;
- redução de símbolos e visibilidade controlada;
- eliminação de código morto e loops redundantes;
- melhor inlining e especialização por arquitetura;
- flags de compilação e linkedição coerentes;
- LTO quando seguro, ou sua desativação quando quebra um backend;
- alinhamento, pré-alocação, prefetch e redução de spills;
- separação correta entre C, C++, TBB e caminhos SIMD.

Os relatórios associam os ganhos a `__restrict__`, `-march=native`, limpeza de
headers e alinhamento. Essa causalidade permanece **hipótese técnica forte**,
mas ainda requer relatório de otimização do compilador, diff de assembly,
`perf stat`, contadores de cache/TLB e commits fixados para ser provada.

## 8. Analogias posteriores no upstream

Depois do encerramento da PR #533, o upstream continuou modificando áreas
operacionalmente próximas, incluindo:

- correção de gating de `g_cpu_features/get_cpu_features` para builds não-x86;
- restrição de C++20 somente ao caminho em que TBB é usado;
- correções de flags SIMD, Cygwin/AVX-512 e LTO;
- ajustes de portabilidade, tipos e infraestrutura C/CMake.

Há, portanto, **analogia objetiva de domínio**: pré-processador, flags,
portabilidade, warnings, build e caminhos SIMD foram temas da contribuição e
também de alterações posteriores.

Isso não prova, por si só, cópia ou derivação. Os estados atuais são:

| Alegação | Estado |
| --- | --- |
| A PR #533 precede essas mudanças posteriores | `CONFIRMADO` |
| Há sobreposição temática e operacional | `CONFIRMADO` |
| Há identidade linha a linha reutilizada sem atribuição | `TOKEN_VAZIO` |
| Houve intenção de apagar autoria | `TOKEN_VAZIO` |
| Cada mudança posterior derivou da PR #533 | `TOKEN_VAZIO` |

`TOKEN_VAZIO` significa ausência de evidência suficiente, nunca conclusão
negativa.

## 9. Fronteira do freestanding

O perfil freestanding/nomalloc atualmente documentado neste fork pertence à
camada autoral externa `rmr/` e foi registrado posteriormente em
`rmr/PROVENIENCE.md`.

Esse mecanismo não aparece, na evidência atualmente indexada, como parte da PR
upstream #533. Assim, a afirmação específica de que o upstream retirou o
freestanding daquela PR permanece `TOKEN_VAZIO` até a identificação de outro
PR, commit ou branch anterior que contenha esse código.

Essa ressalva protege a autoria: ela evita atribuir o mecanismo ao documento
errado e permite registrar a cadeia correta quando o artefato exato for
localizado.

## 10. Economia de datacenter e rack

Os arquivos econômicos são **modelos de cenário**, construídos a partir das
medições de 1 MiB e de premissas de preço, potência, PUE, COP, utilização e
escala. Eles não são medições realizadas em um datacenter real.

Partes diretamente ancoradas no benchmark:

- ciclos e latência aproximadamente `49,3%` menores em 1 MiB;
- p99 aproximadamente `19x` menor;
- CV `282,10% -> 14,75%`;
- throughput `3,01 -> 5,94 GB/s`.

Partes modeladas:

- redução de frota em cargas saturadas;
- energia por hash;
- redução de cooling e CO2;
- economias anuais por rack ou por 50 racks;
- redução de retries e valor de SLA.

Os modelos são úteis para planejamento, mas aplicar a razão de ciclos de 1 MiB
a toda a potência de um rack é uma simplificação. O arquivo de rack contém
inclusive uma recalculação interna do TCO. Por isso:

| Camada | Estado |
| --- | --- |
| Medições x86_64 de ciclos/latência/CV | `MEDIDO_LOCAL` |
| Projeções AWS e rack | `MODELADO` |
| Economia real observada em produção | `TOKEN_VAZIO` |

Artefatos:

- `DATACENTER_ECONOMICS.txt` e `DATACENTER_ECONOMICS-1.txt`
- SHA-256: `0604eaf35f4b739d3912258cdb6432c77bde7bd36ffc561eb0f29c4e6d2f0889`
- `rack_complete_analysis.txt`
- SHA-256: `f3687b71b2381545a53f52b7a806b0c1ce084fcb1e59d5b1a00e7179bdba4023`

## 11. AVX-512, TBB e execução assíncrona

Um ensaio em máquina de 64 núcleos, com afinidade fixa, NUMA controlado,
AVX-512 e TBB, é um próximo experimento válido. Entretanto:

- TBB já existia no upstream antes da PR #533;
- AVX-512 e TBB podem acelerar ambos os lados;
- largura de banda de memória, frequência reduzida sob AVX-512 e overhead de
  threads podem limitar o ganho;
- extrapolar os resultados atuais para esse hardware seria especulação.

AVX-512 é paralelismo SIMD em lockstep; não representa tarefas independentes
terminando em momentos diferentes. O comportamento descrito pelo autor — um
worker que termina cedo continua executando outra tarefa pronta — corresponde
a um escalonador multicore orientado por dependências, normalmente apoiado por
work stealing.

O modelo RMR formalizado evita barreira global por nível da árvore. Cada pai
aguarda somente os dois filhos que formam aquele pai; enquanto um irmão ainda
não chegou, o worker liberado executa outra tarefa pronta. A ordem lógica
`left || right` permanece fixa, preservando determinismo criptográfico.

Artefatos autorais e auditáveis:

- `rmr/docs/ASYNC_PARALLEL_EXECUTION_MODEL.md`;
- `rmr/docs/ASYNC_PARALLEL_BENCHMARK_PROTOCOL.md`;
- registro de autoria e isolamento em `rmr/PROVENIENCE.md`.

Estados atuais:

| Alegação | Estado |
| --- | --- |
| Arquitetura DAG sem barreira global por nível | `ARQUITETURA_ESPECIFICADA` |
| Joins locais esquerdo/direito preservam a árvore | `INVARIANTE_LOGICA` |
| Implementação integral do DAG no caminho de produção | `TOKEN_VAZIO` |
| Resultado em 64 núcleos/TBB/AVX-512 | `TOKEN_VAZIO` |
| Ganho automático por binário monolítico | `TOKEN_VAZIO` |

Um binário único pode favorecer LTO, inlining, internalização e eliminação de
símbolos, mas também pode aumentar pressão de instruction cache e reduzir
portabilidade. Deve ser tratado como hipótese de otimização e medido contra o
mesmo baseline, nunca assumido como superior.

## 12. Compromisso comunitário

Este fork permanece aberto a:

1. reprodução independente dos dois benchmarks;
2. validação bit a bit contra vetores oficiais;
3. publicação de dados brutos e commits fixados;
4. revisão de mudanças pequenas e isoladas;
5. comparação cronológica arquivo a arquivo;
6. atribuição adequada quando houver integração ou reaproveitamento;
7. correção pública de qualquer erro metodológico;
8. reprodução do protocolo assíncrono em hardware multicore adequado.

O pedido não é aceitação automática. É revisão tecnicamente proporcional:

`contribuição + decomposição + teste + comunicação + atribuição -> confiança`
