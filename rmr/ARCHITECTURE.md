# Arquitetura do módulo RMR

Este documento descreve a arquitetura inicial do módulo **RMR** dentro
do repositório BLAKE3. O objetivo é oferecer um espaço isolado para
evoluções focadas em performance, sem impactar o núcleo do hash.

## Objetivos

- **Isolamento**: manter mudanças experimentais contidas em `src/rmr.rs`.
- **Evolução incremental**: permitir upgrades graduais com benchmarks
  antes/depois.
- **Integração controlada**: expor apenas uma interface mínima para
  evitar regressões.

## Componentes

- `src/rmr.rs`: ponto central do módulo, com configuração e versão.
- `rmr/ARCHITECTURE.md`: documentação de arquitetura (este arquivo).
- `rmr/LICENSE_RMR`: licença específica do módulo RMR.

## Diretrizes

1. Qualquer ajuste deve ser acompanhado de testes e benchmarks.
2. Evite dependências adicionais sem necessidade clara.
3. Prefira alterações pequenas e mensuráveis.
