# RMR · arquitetura code-first

Este mapa parte do código, não da narrativa.

## Entrada

- `rmr/core/main.c` chama `rmr_mode_router_main(argc, argv)`.
- `rmr/ui/mode_router.c` seleciona `cli`, `helper` ou `bbs`.
- `cli` chama `pai_main`.

## Comandos reais do PAI

`rmr/core/cli.c` registra:

- `hash`
- `scan`
- `bases`
- `geom`
- `toroid`
- `sign`
- `bench`
- `benchdiff`
- `validate`

## Técnicas implementadas

### `scan`

Gera `manifest.tsv` e uma raiz linear de manifesto. O nome histórico `merkle_root.txt` é mantido por compatibilidade, mas a semântica atual é de raiz linear sobre linhas ordenadas do manifesto.

### `sign`

Assina operacionalmente a saída do scan com hash do binário `pai` e raiz do manifesto.

### `bench`

Executa comandos repetidos, mede tempo, registra métricas TSV/JSONL e cria hashchain FNV-1a leve para rastreabilidade.

### `benchdiff`

Compara duas execuções por média de tempo e sinaliza `OK` ou `ALERT` por limiar percentual.

### `validate`

Valida invariantes internos: alpha, 42 atratores, malha, coprimalidade de passos e métrica simples de entropia.

### `geom` e `toroid`

Geram artefatos visuais simples: PGM e OBJ/MTL. São materializações geométricas auditáveis, não prova científica externa.

## Fronteira ética/técnica

A camada RMR não modifica nem substitui o BLAKE3 upstream. Qualquer integração BLAKE3 futura deve ser opção explícita de hash, preservando SHA-256 atual para compatibilidade.

## Update 2026-05-24 (code-first)
- `linear_manifest_root.txt` é a raiz linear canônica do manifesto.
- `merkle_root.txt` permanece como compatibilidade legado.
- `SIGNATURE.txt` permanece em `PAI SIGNATURE v2` e prioriza `linear_manifest_root.txt` na assinatura.
- `scan` registra algoritmo de hash no manifesto via comentário (`# hash=sha256`).
- `bench` produz `bench.tsv`, `metrics.jsonl`, `summary.json`, `run_manifest.json` e mantém compatibilidade com hashchain FNV-1a.
- `benchdiff` usa média/mediana/p95/variância + contagem de válidos e ok/fail, com status `ALERT` se delta absoluto em média **ou** p95 exceder threshold.
