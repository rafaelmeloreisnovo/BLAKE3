# Privacidade, logs e compliance (RMR + upstream)

Este documento consolida as políticas de privacidade/logs do repositório
BLAKE3, com ênfase no módulo **RMR**, e referencia as licenças aplicáveis.
Ele **não** é uma certificação formal (ISO/NIST/RFC/IEEE/W3C/lei), mas
registra as fronteiras técnicas e de auditoria para apoiar revisões.

## Escopo e coleta de dados

- **Sem telemetria**: o núcleo de hash não coleta nem transmite dados.
  Não há coleta automática no caminho crítico de `src/` e `c/`.
- **Sem logging de dados sensíveis**: entradas, chaves e saídas do hash
  **não devem** ser registradas em stdout/stderr, arquivos ou métricas.

## Limites de logs no repositório

O repositório contém mensagens de diagnóstico **apenas** em exemplos ou
ferramentas auxiliares (ex.: `c/main.c`, `c/example.c`, `c/example_tbb.c`)
para fins de demonstração e teste. O caminho crítico do hash permanece
livre de logs e telemetria.

## Referências de licença (compliance)

- **Upstream BLAKE3**: licenças no topo do repositório
  (`LICENSE_A2`, `LICENSE_A2LLVM`, `LICENSE_CC0`) aplicam-se ao código
  upstream.
- **RMR**: a licença específica do módulo está em `rmr/LICENSE_RMR`.

Para auditorias, mantenha a separação: arquivos sob `rmr/` seguem a
licença RMR; o restante do projeto segue as licenças upstream listadas.
