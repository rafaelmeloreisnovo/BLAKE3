<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see LICENSE_RMR)
-->

# AGENTS.md (Contrato Operacional)

## Escopo
Este arquivo se aplica a toda a árvore do repositório.

## Regras
- Trate o BLAKE3 upstream como referência absoluta (núcleo criptográfico).
- Não altere a lógica criptográfica do núcleo (C/H/ASM/Rust upstream).
- Qualquer código autoral/externo deve ficar isolado em `rmr/` ou `tools/`.
- Mudanças de organização devem ser documentadas.
- Artefatos de build (ex.: `.o`, `.bak`) não devem ser adicionados ao núcleo.

## Auditoria
- Sempre registre diffs contra o upstream oficial ao preparar revisões.
- Documente claramente o que é upstream vs externo em `DOCUMENTACAO.md` e
  `rmr/PROVENIENCE.md`.


## RMR Portable V1 — license exception

`rmr/portable/**` is a prospective project-authored surface. New files in
that tree may use:

```text
SPDX-License-Identifier: LicenseRef-RMR-Individual-Research-1.0
```

instead of the legacy `rmr/LICENSE_RMR`, when explicitly listed by the
portable path/license matrix.

This exception does not relicense upstream BLAKE3 or retroactively revoke
earlier RMR grants. Provider adapters must preserve the upstream boundary.
