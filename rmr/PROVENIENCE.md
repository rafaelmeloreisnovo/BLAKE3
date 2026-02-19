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
| `rmr/include/rmr_governance.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/include/rmr_detect.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/include/rmr_dispatch.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/detect/detect_x86.c` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/detect/detect_aarch64.c` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/detect/detect_fallback.c` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `DOCUMENTACAO.md`, `MANIFESTO*.md` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |

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
