<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see LICENSE_RMR)
-->

# Proveniência do repositório (BLAKE3 vs RMR)

Este documento cataloga a origem por arquivo/pasta para evitar mistura de
autoria entre o upstream **BLAKE3** e o módulo **RMR** (Rafael Melo Reis).
Ele complementa a seção de proveniência em `DOCUMENTACAO.md`.

## Regras de isolamento

- **Upstream BLAKE3** permanece com autoria e licenças originais.
- **RMR autoral** permanece isolado e identificado explicitamente.
- Integrações entre blocos devem ser **mínimas, documentadas e explícitas**.

## Mapa de proveniência (por arquivo/pasta)

| Caminho | Proveniência | Licença aplicável |
| --- | --- | --- |
| `src/` (exceto `src/rmr.rs`) | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `src/rmr.rs` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `c/` (exceto `c/rmr_lowlevel.h` e `c/rmr_arch.h`) | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `c/rmr_lowlevel.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `c/rmr_arch.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `b3sum/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `reference_impl/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `test_vectors/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `benches/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `tools/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `media/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `rmr/` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `README.md`, `CONTRIBUTING.md`, `LICENSE_*`, `Cargo.toml`, `Cargo.lock`, `build.rs` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `DOCUMENTACAO.md`, `MANIFESTO*.md` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |

## Observações sobre integração

- O uso de `src/rmr.rs` ou `c/rmr_*.h` em outros arquivos **não altera a
  proveniência** desses arquivos: eles seguem sendo upstream, apenas
  consumindo interfaces explicitamente marcadas como RMR.
- Qualquer novo arquivo autoral do RMR **deve**:
  1. Ficar dentro de `rmr/` ou ser adicionado à lista de exceções acima.
  2. Incluir cabeçalho de copyright/licença no próprio arquivo.
  3. Ser registrado neste documento.
