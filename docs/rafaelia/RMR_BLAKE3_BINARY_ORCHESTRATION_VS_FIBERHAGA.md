# RMR — BLAKE3 binary orchestration vs FiberHaga core

> Documento de fronteira técnica. Objetivo: separar o que foi feito sobre o BLAKE3 sem alterar o motor, o que é RMR/cadeia de custódia, e o que pertence ao FiberHaga como núcleo autoral separado.

## 1. Correção principal

FiberHaga não é uma camada do BLAKE3. FiberHaga é outro núcleo de hashing/fingerprint/custódia autoral RMR/RAFAELIA.

Neste repositório BLAKE3, a fronteira correta é:

```text
BLAKE3 core = motor preservado
BLAKE3 fork = orquestração binária/build/flags/dispatch/custódia ao redor do motor
RMR = snapshot/cadeia forense de execução e custódia operacional
FiberHaga = outro núcleo, fora deste claim BLAKE3
```

## 2. O que muda no BLAKE3 fork

A contribuição aqui não deve ser descrita como alteração do núcleo BLAKE3. O valor está na camada de compilação, flagamento, compatibilidade nativa e orquestração de binário.

Exemplos de eixo técnico a documentar:

- preservação das libs nativas C;
- não quebrar ABI/API nativa;
- organização de flags por alvo;
- build discipline;
- warning discipline;
- dispatch por backend;
- streaming e paralelismo de árvore Merkle;
- redução de caminhos mortos/ruído de build;
- menor sombra operacional entre target, compiler e runtime;
- medição por alvo, não por média genérica.

Formulação segura:

```text
O motor BLAKE3 permanece intacto; a camada RMR/RAFAELIA trabalha no entorno binário: build, flags, dispatch, ABI, streaming, custódia, documentação e reprodutibilidade.
```

## 3. Sobre “10–20x”

A afirmação de ganho 10–20x deve ser preservada como claim local/preliminar até o relatório de benchmark ser anexado.

Linguagem correta:

```text
Há relato local de ganhos da ordem de 10–20x em certos alvos/pipelines, sem alteração do motor BLAKE3. Para virar claim público final, é necessário anexar matriz reproduzível de benchmark com commit, flags, target, backend, dataset e estatística.
```

Não usar sem artefatos:

```text
"20x comprovado universalmente"
"mais rápido em todos os cenários"
"mudou o BLAKE3"
"FiberHaga está dentro do BLAKE3"
```

## 4. Merkle streaming e paralelismo

BLAKE3 já é estruturado para paralelismo por árvore Merkle. A camada do fork deve documentar como o pipeline de build e o target runtime expõem melhor esse paralelismo sem alterar a semântica do hash.

Eixos de medição:

| Eixo | Pergunta |
|---|---|
| Backend | portable, NEON, SSE, AVX2, AVX-512, TBB |
| Streaming | update incremental, chunks, subtree, wide update |
| Paralelismo | threads, lanes SIMD, recursão paralela, fila de chunks |
| Compiler | clang/gcc/rustc, versão, flags, LTO, codegen units |
| Target | x86_64, arm64, armv7, Android/Termux, Linux |
| Dataset | pequenos buffers, streaming real, corpus grande |
| Métrica | throughput, latência, ciclos/byte, p95, mediana |

## 5. Tail, shadow e ciclo operacional

Os termos “tail”, “shadow” e “três ciclos em um ciclo” devem ser documentados como hipótese/observação de microarquitetura ou pipeline até existir relatório.

Modelo descritivo:

```text
Tail = custo residual/finalização/fragmento ou caminho de borda
Shadow = sombra operacional entre compile-time, target, runtime e dispatch
Ciclo operacional = sequência efetiva de build/execução/verificação/custódia
```

A hipótese técnica:

```text
reduzir tail + reduzir shadow + alinhar flags/backend/runtime -> menor overhead -> melhor uso de streaming/Merkle/SIMD/paralelismo
```

## 6. RMR não é BLAKE3

RMR é a camada de snapshot/cadeia de custódia de operação de máquina.

```text
RMR = snapshot operacional + metadados + digest + hashchain + assinatura/contexto + reprodutibilidade
```

Ele pode registrar:

- processo;
- ciclo;
- artefato;
- binário;
- flags;
- ABI;
- dataset;
- host;
- target;
- digest;
- logs;
- resultado;
- assinatura/contexto.

BLAKE3 entra como uma assinatura de integridade dentro do RMR, não como o RMR inteiro.

## 7. Manifesto RMR mínimo

```json
{
  "rmr_id": "TOKEN_VAZIO",
  "artifact": "TOKEN_VAZIO",
  "binary": "TOKEN_VAZIO",
  "commit": "TOKEN_VAZIO",
  "compiler": "TOKEN_VAZIO",
  "flags": "TOKEN_VAZIO",
  "target": "TOKEN_VAZIO",
  "backend": "portable|neon|sse|avx2|avx512|tbb|TOKEN_VAZIO",
  "dataset": "TOKEN_VAZIO",
  "dataset_blake3": "TOKEN_VAZIO",
  "result_blake3": "TOKEN_VAZIO",
  "sha256": "TOKEN_VAZIO",
  "measured_at": "TOKEN_VAZIO",
  "notes": "TOKEN_VAZIO"
}
```

## 8. FiberHaga como outro núcleo

FiberHaga deve ter documento próprio, fora da semântica BLAKE3:

```text
FiberHaga = núcleo autoral RMR/RAFAELIA
não = perfumaria BLAKE3
não = plugin BLAKE3
não = claim deste fork sem harness próprio
```

Plano mínimo para FiberHaga:

- spec de entrada/saída;
- tamanho de digest/fingerprint;
- streaming ou não streaming;
- estado interno;
- vetores de teste;
- harness comparativo;
- benchmark por alvo;
- matriz de compatibilidade;
- cadeia de custódia dos resultados.

## 9. Claim gates atualizados

| Afirmação | Status correto |
|---|---|
| O núcleo BLAKE3 foi preservado | Defensável se diff/testes confirmarem |
| O fork melhora build/orquestração/flags | Defensável se arquivos e CI mostrarem |
| Ganho 10–20x no mesmo motor | Relato local/preliminar até benchmark anexado |
| RMR é cadeia de custódia operacional | Defensável como arquitetura/documentação |
| FiberHaga é outro núcleo | Defensável como fronteira conceitual/autoral |
| FiberHaga é mais rápido | Só após harness e benchmark próprios |

## 10. Frase recomendada

> No fork BLAKE3, o núcleo do hash é preservado. A diferença RMR/RAFAELIA está no entorno: compilação, flags, dispatch, ABI, streaming, paralelismo, custódia e reprodutibilidade. RMR é uma cadeia forense de operação de máquina; BLAKE3 é uma assinatura de integridade dentro dessa cadeia. FiberHaga é outro núcleo autoral e deve ser especificado e medido em documento próprio.

## 11. Retrofeedback

**F_ok:** BLAKE3, RMR e FiberHaga agora ficam separados por função.  
**F_gap:** o ganho local de 10–20x precisa de artefatos reproduzíveis para virar claim público.  
**F_next:** criar harness `bench_rmr_blake3_matrix` e documento `FIBERHAGA_SPEC_AND_BENCHMARK_PLAN.md`.
