<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see rmr/LICENSE_RMR)
-->

# Revisão de determinismo (rmr/ e c/)

Este documento define uma **lista de verificação** para auditorias no
código-fonte em `rmr/` e `c/`, com foco em determinismo e previsibilidade
em todos os caminhos críticos do hash. Ele não é uma certificação formal,
mas orienta revisões para garantir que os resultados sejam esperados e
reprodutíveis.

## Objetivo

Garantir que qualquer caminho de execução do hash seja determinístico:
mesma entrada + mesmo contexto de build ⇒ mesma saída.

## Contrato normativo para hot path

- Documento normativo: `rmr/docs/HOTPATH_CONTRACT.md`.
- Referência de governança em código: `rmr/include/rmr_governance.h`
  (`RMR_HOTPATH_CONTRACT_DOC` e `RMR_HOTPATH_CONTRACT_VERSION`).
- Toda revisão deve validar explicitamente os invariantes por camada
  (`rmr/hwif/asm/*`, `rmr/hwif/rmr_hwif.c`, `rmr/core/*`).

## Lista de verificação (auditoria)

1. **Sem fontes de aleatoriedade**
   - Nenhuma chamada a `rand()`, geradores de entropia, relógio, ou
     dependências temporais em `rmr/` e `c/`.
2. **Sem IO no caminho crítico**
   - Não há leitura/escrita de arquivos, rede ou stdout/stderr na lógica
     do hash (mensagens ficam restritas a exemplos/testes).
3. **Sem estado global mutável compartilhado**
   - Evitar estado global que altere o resultado entre execuções.
4. **Determinismo por arquitetura**
   - As rotas SIMD/ASM e o dispatcher devem produzir o mesmo resultado
     que a implementação portátil, para os mesmos vetores de teste.
5. **Paralelismo controlado**
   - O agendamento de threads não deve alterar a saída do hash.
6. **Tratamento de erros previsível**
   - Erros devem falhar de forma consistente, sem comportamento
     dependente de timing ou condições externas.
7. **Sem chamadas externas no ASM de backend**
   - `rmr/hwif/asm/*` deve manter hot path ABI-safe e sem `call`/`bl`.
8. **Sem heap em loop crítico de `rmr/core/*`**
   - Se inevitável, alocação deve estar concentrada fora do loop e com
     justificativa técnica no código.
9. **Dispatcher leve e lock-free**
   - `rmr/hwif/rmr_hwif.c` deve manter seleção atômica/lock-free sem
     lógica pesada e sem IO.

## Evidências recomendadas

- Rodar vetores de teste oficiais e comparar com a implementação de
  referência.
- Revisar diffs em `rmr/` e `c/` buscando efeitos colaterais ou logs.
- Executar triagem automática mínima:

```sh
rg -n "printf|fprintf|fopen|write\(|send\(|recv\(|socket\(" rmr/hwif rmr/core
rg -n "malloc\(|calloc\(|realloc\(|free\(" rmr/core
rg -n "\b(call|bl)\b" rmr/hwif/asm
```
