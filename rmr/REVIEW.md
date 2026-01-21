# Revisão de determinismo (rmr/ e c/)

Este documento define uma **lista de verificação** para auditorias no
código-fonte em `rmr/` e `c/`, com foco em determinismo e previsibilidade
em todos os caminhos críticos do hash. Ele não é uma certificação formal,
mas orienta revisões para garantir que os resultados sejam esperados e
reprodutíveis.

## Objetivo

Garantir que qualquer caminho de execução do hash seja determinístico:
mesma entrada + mesmo contexto de build ⇒ mesma saída.

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

## Evidências recomendadas

- Rodar vetores de teste oficiais e comparar com a implementação de
  referência.
- Revisar diffs em `rmr/` e `c/` buscando efeitos colaterais ou logs.

