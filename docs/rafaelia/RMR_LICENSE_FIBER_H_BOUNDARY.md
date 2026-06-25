# RMR — licenças, fronteira BLAKE3 e limite Fiber H

> Documento complementar para separar a licença/base upstream BLAKE3, a camada de build/custódia RAFAELIA e o Fiber H como método autoral distinto.

## 1. Diagnóstico do repositório

Este fork contém duas licenças relevantes:

- `LICENSE_A2` — Apache License 2.0;
- `LICENSE_CC0` — CC0 1.0 Universal.

A presença das duas licenças deve ser tratada como fronteira documental. Licença permite uso e redistribuição conforme seus termos; licença não prova performance, qualidade, autoria de camadas externas ou validade científica de claims novos.

## 2. Apache 2.0

A licença Apache 2.0 permite uso, reprodução, preparação de trabalhos derivados, exibição, sublicenciamento e distribuição em source ou object form, sob as condições da licença. Ela também inclui uma seção de licença de patente nos limites definidos pelo texto.

Uso correto neste fork:

```text
BLAKE3 upstream -> licença permissiva -> fork documentado -> build/custody/orchestration RAFAELIA
```

A camada RAFAELIA deve manter atribuição, fronteira e compatibilidade quando usar código BLAKE3.

## 3. CC0

A licença CC0 expressa intenção de dedicar o trabalho ao commons e permitir reutilização ampla, inclusive comercial, na maior extensão permitida. O próprio texto preserva limitações relevantes: não libera automaticamente marca/patente e não garante direitos de terceiros.

Uso correto neste fork:

```text
CC0 = liberdade ampla de uso do material coberto
não = prova de certificação, performance, segurança adicional ou autoria de camadas externas
```

## 4. Fronteira técnica correta

O README do fork já define a fronteira adequada:

```text
BLAKE3 = primitivo de hash
RAFAELIA = build orchestration + custody + metadata + symbolic indexing + reproducibility context
```

Não usar:

```text
"BLAKE3 foi substituído por RAFAELIA"
"este fork é globalmente superior ao upstream"
"a licença prova superioridade técnica"
```

Usar:

```text
"BLAKE3 permanece como primitivo de hash; o fork adiciona/documenta camadas de build, flags, dispatch, warning discipline, TBB opcional e custódia RAFAELIA."
```

## 5. Fiber H não é perfumaria do BLAKE3

Fiber H deve ser tratado como método autoral separado.

```text
BLAKE3 = primitivo público, permissivo, com especificação e vetores conhecidos
Fiber H = método autoral RMR/RAFAELIA a ser especificado, testado e comparado por harness próprio
```

Não declarar Fiber H como “BLAKE3 melhorado” sem especificação formal. Declarar como camada/método separado:

```text
Fiber H é um candidato autoral de hashing/fingerprint/custódia que deve ser avaliado contra BLAKE3 e outros métodos por matriz de benchmarks, vetores, throughput, latência, streaming, memória, ABI e reprodutibilidade.
```

## 6. Estimativa 10–20x

A estimativa de 10–20x pode existir como hipótese operacional, mas não como claim final.

Linguagem segura:

```text
Hipótese de performance: em certos alvos, flags e pipelines de build/orquestração, ganhos de ordem 10–20x podem ser investigados como estimativa preliminar. Essa estimativa não é claim de produção até existir benchmark reproduzível.
```

Linguagem proibida sem matriz:

```text
"10–20x comprovado"
"superior ao BLAKE3 upstream em todos os cenários"
"melhor que BLAKE3 em termos universais"
```

## 7. Benchmark matrix mínima

Para transformar estimativa em evidência, registrar:

| Campo | Obrigatório |
|---|---|
| Commit | SHA exato do fork e baseline comparado |
| Host | CPU, arquitetura, RAM, sistema operacional |
| Target | x86_64, arm64, armv7, Android/Termux etc. |
| Compilador | clang/gcc/rustc, versão, flags |
| Backend | portable, SSE, AVX2, AVX-512, NEON, TBB |
| Dataset | tamanhos: 64B, 1KiB, 1MiB, 1GiB; sintético/real |
| Métricas | throughput, latência, ciclos/byte, memória |
| Execuções | N repetições, mediana, p95, desvio |
| Integridade | digest esperado, hash do dataset, hash do relatório |
| BLAKE3 digest | hash do relatório/artefatos com BLAKE3 |
| SHA256 | hash secundário para interoperabilidade |

## 8. Fiber H claim gate

| Claim | Status correto |
|---|---|
| Fiber H é autoral | Defensável se houver spec própria e autoria documentada |
| Fiber H é mais rápido em alvo X | Só após benchmark reproduzível em alvo X |
| Fiber H é melhor para RAFAELIA custody | Defensável como hipótese se o uso for fingerprint/custódia e houver medição |
| Fiber H substitui BLAKE3 universalmente | Não declarar sem prova muito forte |

## 9. Modelo de relação

```text
BLAKE3 upstream
  -> primitivo permissivo e auditável
  -> fork RAFAELIA documenta build/custody/orchestration
  -> Fiber H permanece método autoral paralelo
  -> benchmarks e hashchains conectam as evidências
```

## 10. Frase recomendada

> Este fork não reivindica que a camada RAFAELIA altere a semântica do BLAKE3. O fork preserva BLAKE3 como primitivo e adiciona documentação/infraestrutura de build, custódia, metadados, reprodutibilidade e auditoria. Fiber H é tratado como método autoral separado, a ser especificado e comparado por matriz de benchmark própria.

## 11. Retrofeedback

**F_ok:** duas licenças identificadas; fronteira BLAKE3/RAFAELIA/Fiber H documentada.  
**F_gap:** Fiber H precisa de spec, vetores e benchmark matrix antes de claims fortes.  
**F_next:** criar `docs/rafaelia/FIBER_H_SPEC_AND_BENCHMARK_PLAN.md` e harness comparativo BLAKE3 x baseline x Fiber H.
