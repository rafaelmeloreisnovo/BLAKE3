# RMR/BLAKE3 · plano de implementação

Este arquivo registra pendências técnicas da camada `rmr/` sem alterar o core upstream BLAKE3.

## Fronteira

- `src/`, `c/`, `b3sum/`, `reference_impl/` e `test_vectors/` permanecem upstream.
- `rmr/` é camada externa de auditoria, benchmark, assinatura e geometria.

## Pendências coerentes

1. Clarificar `merkle_root.txt`: o `scan.c` atual gera raiz linear do manifesto, não uma árvore Merkle binária completa.
2. Aceitar caminho explícito do binário em `pai sign`, por exemplo `--self ./pai`.
3. Ampliar `benchdiff` para mediana, p95 e variância, não apenas média.
4. Criar teste canônico de pipeline determinístico.
5. Integrar BLAKE3 como opção de hash futura sem quebrar SHA-256 atual.

## Regra

Toda mudança deve ficar na camada `rmr/` e preservar a semântica do BLAKE3 upstream.
