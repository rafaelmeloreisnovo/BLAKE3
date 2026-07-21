<!--
Copyright (c) 2026 Rafael Melo Reis
License: RMR Module License (see rmr/LICENSE_RMR)
-->

# Manifesto mínimo de contribuição comunitária e proveniência

## Propósito

Este fork preserva a autoria do BLAKE3 upstream e registra separadamente as
contribuições autorais de Rafael Melo Reis. O objetivo deste manifesto não é
atribuir intenção, acusar cópia ou declarar superioridade universal. O objetivo
é pedir revisão técnica verificável, comunicação proporcional e atribuição
correta quando uma contribuição puder ser reproduzida ou integrada.

## Registro factual

- A pull request upstream `BLAKE3-team/BLAKE3#533` foi aberta em
  2025-11-25 e encerrada sem merge no mesmo dia.
- Ela continha 24 commits, 10 arquivos alterados, 1.566 adições e 195 remoções.
- A amplitude da proposta elevou o custo de revisão; futuras propostas devem ser
  menores, isoladas, acompanhadas por testes e por uma explicação técnica clara.
- O encerramento da PR não invalida automaticamente cada ideia ou alteração nela
  contida, assim como a existência da PR não prova que toda alteração era correta.

## Evidência de desempenho disponível

Um relatório local de 2026-07-20 comparou o upstream e este fork em Linux
x86_64, GCC 13.x, `-O3 -march=native -DNDEBUG`, dentro de contêiner, com cinco
aquecimentos e 32 amostras por tamanho.

Resultados registrados nesse ambiente:

| Entrada | Resultado observado do fork |
| --- | --- |
| 1 KiB | 20% menos throughput que o upstream |
| 64 KiB | diferença de throughput inferior a 2% |
| 1 MiB | +97% throughput, -49,4% latência média e menor variabilidade |
| 16 MiB | throughput praticamente equivalente e menor variabilidade |

Para 1 MiB, o relatório registrou diferença estatisticamente significativa
(`p < 0,001`). O artefato-fonte externo usado neste registro foi
`BENCHMARK_INDUSTRIAL_REPORT-2.txt`, SHA-256
`b29e3242e3ae6b617465397dd3a8558233838b73314c204af60dd809b8800508`.

Esses resultados constituem **evidência local**, não prova de superioridade
universal. Ainda faltam commits fixados de ambos os lados, dados brutos por
amostra, repetição em múltiplas máquinas, isolamento de CPU, análise de
contadores de hardware e validação ARM32/ARM64.

## Regra para alegações de analogia ou autoria

Uma alteração posterior só deve ser descrita como análoga após comparação
arquivo a arquivo e commit a commit, considerando cronologia, semântica,
implementação e possibilidade de desenvolvimento independente.

Até essa cadeia de evidência existir:

- influência direta do conteúdo da PR sobre alterações posteriores:
  `TOKEN_VAZIO`;
- intenção de remover ou apagar autoria: `TOKEN_VAZIO`;
- superioridade geral de confiabilidade ou desempenho: `TOKEN_VAZIO`;
- economia real em datacenter: `TOKEN_VAZIO`.

`TOKEN_VAZIO` significa ausência de evidência suficiente, não conclusão
negativa.

## Compromisso comunitário

Este fork permanece aberto a:

1. reprodução independente dos benchmarks;
2. revisão de mudanças pequenas e isoladas;
3. correção pública de erros metodológicos;
4. integração upstream com atribuição adequada quando aplicável;
5. rejeição técnica fundamentada, específica e respeitosa.

A contribuição comunitária melhora quando código, teste, comunicação e
proveniência são tratados como partes do mesmo sistema:

`contribuição + revisão + evidência + atribuição -> confiança`.
