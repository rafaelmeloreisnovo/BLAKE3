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

## Update 2026-05-24
- `scan` suporta `--hash sha256` (default) e grava `# hash=sha256` no topo do `manifest.tsv`.
- `--hash blake3` retorna erro explícito como pendência de backend C externo limpo (sem invasão do core upstream).
- `bench` mantém hashchain FNV-1a e adiciona fingerprints SHA-256 de artefatos no `run_manifest.json`.
- `summary.json` inclui `min_ms`, `max_ms`, `median_ms`, `p95_ms`, `variance`, `ok_count`, `repeat`, `final_run_hash`.
- `benchdiff` passa a avaliar média + p95 (threshold em delta absoluto) e emite `benchdiff_report.json`.
- Scripts canônicos:
  - `rmr/tools/build_pai.sh`
  - `rmr/tools/run_full_audit.sh`


## Update 2026-09-21 — BLAKE3 backend and reproducibility closure

The previously documented backend gap is addressed by an explicit external adapter:
- rmr/core/hash_blake3.c consumes the public c/blake3.h API;
- rmr/CMakeLists.txt links the RMR executable against BLAKE3::blake3;
- pai hash accepts --algo blake3;
- pai scan accepts --hash blake3;
- the upstream primitive remains outside rmr/.

The comparison path is materialized by rmr/tools/orchestrate_blake3_compare.sh and the common harness in rmr/benchmark_framework/core/blake3_bench.c.

Promotion states are intentionally split:
- source implementation: IMPLEMENTED;
- local build/test: requires observable run;
- GitHub CI reproduction: requires workflow PASS;
- independent third-party reproduction: TOKEN_VAZIO until external receipt.

This update closes an implementation gap; it does not establish a performance winner.
