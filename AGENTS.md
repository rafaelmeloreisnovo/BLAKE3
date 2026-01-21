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
