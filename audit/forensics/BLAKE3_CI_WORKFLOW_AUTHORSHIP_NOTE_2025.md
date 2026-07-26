# BLAKE3 — nota de autoria do workflow CI na PR #533

Data: 2026-07-26

## Questão

O workflow que apareceu e rodou durante a PR #533 foi criado/modificado pela contribuição de Rafael Melo Reis ou era o workflow oficial já existente?

## Evidência

A lista de dez caminhos modificados pela PR #533 não contém:

```text
.github/workflows/ci.yml
```

O arquivo `.github/workflows/ci.yml` na base da PR:

```text
base_sha=308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
workflow_blob_sha=6254b10042926c6026ff26eed879a9c0c85ed170
```

O mesmo arquivo no head da PR:

```text
head_sha=15829f851e45d1327b017be67a7b88d7725bc653
workflow_blob_sha=6254b10042926c6026ff26eed879a9c0c85ed170
```

Os blobs são idênticos. Portanto, na PR #533:

```text
workflow_created_by_PR533=NO
workflow_modified_by_PR533=NO
official_workflow_executed_PR533_head=YES
```

## Formulação correta

> A contribuição de Rafael não introduziu nem alterou o `ci.yml` na PR #533. A PR acionou o workflow oficial `tests`, já existente na base, e esse workflow executou a árvore do head da contribuição.

Isso continua sendo evidência documental importante: o código submetido entrou na matriz oficial de CI. Porém, não permite atribuir a autoria daquele YAML ao autor da PR.

## Escopo aberto

O autor relata outro workflow/fork anterior em 2025. A existência e autoria desse workflow anterior permanecem:

```text
old_fork_workflow=TOKEN_VAZIO
repository_or_archive=TOKEN_VAZIO
commit_sha=TOKEN_VAZIO
```

O backup Termux de maio de 2025 é o candidato prioritário para procurar esse artefato.
