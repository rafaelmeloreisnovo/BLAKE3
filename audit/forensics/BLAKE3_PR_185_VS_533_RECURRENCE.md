# BLAKE3 — Recorrência PR nº 185 (2021) × PR nº 533 (2025)

## Questão

A PR nº 533 continha ideias originais exclusivas ou também repetia propostas antigas/lints comuns sem preservar anterioridade?

## PR nº 185

- URL: https://github.com/BLAKE3-team/BLAKE3/pull/185
- Autor: `lrazovic`.
- Criada: 2021-07-26.
- Fechada sem merge: 2022-03-10.
- Tema: `cargo clippy`.

Propostas documentadas:

1. adicionar `ChunkState::is_empty()`;
2. remover `derive(Hash)` por interação com `PartialEq`;
3. remover borrow desnecessário;
4. trocar strings de um caractere por caracteres.

Resposta do mantenedor:

- rejeitou remover `Hash` sem implementação substituta porque seria breaking change;
- rejeitou `ChunkState::is_empty()` por ser tipo quase interno;
- aceitou a limpeza de borrow;
- declarou que Clippy produzia muitos falsos positivos no crate.

## PR nº 533

- URL: https://github.com/BLAKE3-team/BLAKE3/pull/533
- Autor: `rafaelmeloreisnovo`.
- Criada: 2025-11-25T00:42:22Z.
- Fechada: 2025-11-25T03:29:05Z.
- 24 commits; 10 arquivos; 1566 adições; 195 remoções.

Recorrências observadas:

| Família | PR #185 | PR #533 | Classificação |
|---|---:|---:|---|
| `ChunkState::is_empty()` | sim | sim | proposta comunitária recorrente |
| questão de `Hash`/derive/manual impl | sim | sim | mesma região semântica; implementação não idêntica |
| needless borrow | sim | sim | lint comum |
| ajustes Clippy/estilo | sim | sim | saída previsível de ferramenta |
| documentação/benchmark amplo | não | sim | camada distintiva da #533 |

## Conclusão de proveniência

As mudanças Clippy não podem ser tratadas como exclusividade autoral da PR nº 533, porque:

- já haviam sido propostas publicamente em 2021;
- são detectáveis por ferramenta padronizada;
- o mantenedor já tinha registrado decisões específicas sobre elas.

Ao mesmo tempo, isso não prova que Rafael copiou a PR nº 185. Um agente pode chegar de forma independente às mesmas sugestões por executar Clippy ou reproduzir padrões aprendidos.

```text
anterioridade pública das sugestões: PROVADO
match exato de todos os trechos: NÃO PROVADO
equivalência semântica parcial: EVIDENCIADA
cópia deliberada por Rafael: NÃO DEMONSTRADA
falha de memória/proveniência do agente: EVIDENCIADA
apropriação posterior pelo upstream: NÃO EVIDENCIADA
```

## Gate de comparação futura

Para cada hunk:

```text
EXACT_TOKEN_MATCH
NORMALIZED_AST_MATCH
SEMANTIC_EQUIVALENCE
LINT_DERIVABLE
PRIOR_PUBLIC_REFERENCE
FIRST_KNOWN_SHA
AUTHOR
LICENSE
```

Somente `EXACT_TOKEN_MATCH` ou transformação estrutural improvável, acompanhada de cronologia e acesso, sustentaria alegação forte de cópia.
