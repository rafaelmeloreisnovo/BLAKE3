<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see LICENSE_RMR)
-->

# Proveniência do repositório (BLAKE3 vs RMR)

Este documento cataloga a origem por arquivo/pasta para evitar mistura de
autoria entre o upstream **BLAKE3** e a camada externa **RMR**.

## Regras de isolamento

- **Upstream BLAKE3** permanece com autoria e licenças originais.
- **RMR autoral** permanece isolado em `rmr/`.
- Integrações entre blocos devem ser **mínimas, documentadas e explícitas**.

## Mapa de proveniência (por arquivo/pasta)

| Caminho | Proveniência | Licença aplicável |
| --- | --- | --- |
| `src/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `c/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `b3sum/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `reference_impl/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `test_vectors/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `benches/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `tools/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `media/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `README.md`, `CONTRIBUTING.md`, `LICENSE_*`, `Cargo.toml`, `Cargo.lock`, `build.rs` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `rmr/` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/benchmark_framework/` | RMR autoral (blueprint) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/ui/` | RMR autoral (front controller de modos) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/include/rmr_governance.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/hwif/include/rmr_hwif.h`, `rmr/hwif/rmr_hwif.c`, `rmr/hwif/asm/aarch64/`, `rmr/hwif/asm/x86_64/` | RMR autoral (interface HW + backends ASM) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/core/validate.c`, `rmr/core/pai_validate.h` | RMR autoral (validação determinística de invariantes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `DOCUMENTACAO.md`, `MANIFESTO*.md` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/MANIFESTO_RAFAELIA.md` | RMR autoral (texto não jurídico) | RMR Module License (`rmr/LICENSE_RMR`) |

## Itens fora de `rmr/` sob autoria externa

Os itens abaixo são **externos ao upstream** e ficam fora de `rmr/` por decisão
organizacional. Eles devem manter rastreabilidade de licença e finalidade:

| Caminho | Origem | Licença aplicável | Finalidade |
| --- | --- | --- | --- |
| `DOCUMENTACAO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Fronteiras de escopo, operação e integração |
| `MANIFESTO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Manifesto e diretrizes de projeto |
| `MANIFESTO_RAFAELIA.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Manifesto complementar/autoral |
| `FORK_NOTES.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Notas de distribuição do fork |
| `RELATORIO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Relatório técnico consolidado |
| `RELATORIO_AUDITORIA.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Relatório de auditoria e conformidade |
| `AGENTS.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Governança operacional para agentes |
| `tools/check_rmr_headers.py` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Verificação de cabeçalho `LICENSE_RMR`, shebang e escopo de alterações |
| `tools/check_rmr_headers.sh` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Wrapper para execução do verificador Python |
## Observações sobre integração

- O RMR é **externo** e não integra o núcleo BLAKE3 por padrão.
- Qualquer novo arquivo autoral do RMR **deve**:
  1. Ficar dentro de `rmr/`.
  2. Incluir cabeçalho de copyright/licença no próprio arquivo.
  3. Ser registrado neste documento.
  4. Ser isolado do core (sem modificar `src/` ou `c/`).
Compliance Notice

All licensing and provenance decisions in this repository
are manually reviewed by the author.

No automated tool or AI system is authorized to alter
licensing terms, introduce legal assumptions, or publish
code on behalf of the author.

## Separação entre conteúdo jurídico e não jurídico

- `rmr/LICENSE_RMR` deve conter apenas o texto legal da licença RMR.
- Conteúdos técnicos/conceituais não jurídicos ficam em
  `rmr/MANIFESTO_RAFAELIA.md`.


## Nota de auditoria (benchmark telemetry store)

Mudanças de telemetria/armazenamento de benchmark permanecem restritas a `rmr/` (ex.: `rmr/core/bench.c`, `rmr/benchmark_framework/README.md`, `rmr/ui/mode_router.c`) e não alteram o núcleo criptográfico upstream (`src/`, `c/`, `reference_impl/`).

### Atualização 2026-05-02 (governança de telemetria)

Extensões de governança/telemetria foram aplicadas apenas em `rmr/include/rmr_governance.h` e `rmr/core/bench.c`, mantendo isolamento no módulo RMR e sem alterações no núcleo BLAKE3 upstream (`src/`, `c/`, `reference_impl/`).

### Atualização 2026-05-02 (validação RMR)

Adicionado comando `pai validate` (arquivos `rmr/core/validate.c` e `rmr/core/pai_validate.h`) para validar invariantes matemáticos/operacionais do módulo RMR em runtime, mantendo isolamento da camada externa e sem alterar o núcleo criptográfico upstream.
