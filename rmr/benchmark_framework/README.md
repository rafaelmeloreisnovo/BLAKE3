<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see LICENSE_RMR)
-->

# RMR Benchmark Framework (Blueprint)

Este diretório contém o **blueprint** do framework de benchmark industrial do
RMR, **isolado do núcleo BLAKE3 upstream**. Nenhuma lógica criptográfica do
BLAKE3 é alterada aqui; o framework deve consumir o BLAKE3 apenas como
biblioteca (via CLI, bindings ou FFI), mantendo separação total entre
**CORE** e **RMR/EXTERNAL**.

## Objetivos

- Medir performance de forma justa (RAM, IO e Pipeline), com 5 runs e mediana.
- Evitar métricas falsas ou enviesadas (sem logs extras, sem IO oculto).
- Permitir parametrização total por CLI ou configuração.
- Garantir reprodutibilidade (seed, tamanho, threads e ambiente registrados).
- Ter **opções de CLI e BBS** (interface textual estilo BBS/TUI).

## Estrutura proposta

```
rmr/benchmark_framework/
  README.md            # Este documento
  cli/                 # CLI principal (rmr-bench)
  core/                # Núcleo do framework (sem criptografia)
  metrics/             # Métricas e agregadores
  output/              # Exportadores JSON/CSV/Markdown
  configs/             # Perfis e configs versionadas
```

## Interfaces

### CLI (obrigatória)

Exemplos de uso:

```
rmr-bench --profile ram --size 4GiB --runs 5 --seed 123
rmr-bench --profile io --file ./big.dat --runs 5 --threads 1
rmr-bench --profile pipeline --runs 5 --save report.json
```

### Front controller (`rmr`)

Interface unificada proposta com parser central (`rmr/ui/mode_router.c`):

```bash
rmr run --mode cli -- hash --file ./amostra.bin
rmr run --mode helper
rmr run --mode bbs
```

- `--mode cli`: encaminha argumentos para o backend `pai_main`.
- `--mode helper`: wizard por etapas (perfil, input, saída, confirmação).
- `--mode bbs`: menu textual com navegação por teclado e fallback para terminal simples.

### BBS / TUI (obrigatória)

- Interface textual em modo “BBS” (menus, navegação por teclado).
- Permite escolher perfis, tamanhos, seed, threads e formato de saída.
- Não executa automaticamente sem confirmação explícita do usuário.

## Perfis de benchmark (mínimo)

- **ram**: hash de buffer grande (1–4 GiB), sem IO, sem hex.
- **io**: leitura de arquivo + hash, sem logs extras.
- **pipeline**: pipeline RMR completo (scan/benchdiff/etc.).

## Famílias de métricas (base)

- Throughput (GiB/s)
- Latência (tempo total e por etapa)
- Variância / dispersão
- Overhead (setup vs execução)
- Consistência (verificação de erro/ruído)

> Nota: o framework poderá evoluir até ~42 métricas, mas começa com o mínimo
> essencial auditável.

## Saídas

- JSON (máquina)
- CSV (planilhas)
- Markdown (relatórios)

## Reprodutibilidade

- Registrar: seed, tamanho, threads, temperatura (se disponível), frequência,
  flags, commit hash e timestamp.

## Isolamento do core

- Nenhum arquivo do núcleo BLAKE3 deve ser modificado.
- Toda evolução aqui permanece **estritamente em `rmr/`**.


## Armazenamento operacional (`output/`)

Sessões de benchmark agora podem persistir telemetria em:

- `run_manifest.json`: configuração, ambiente, `snapshot_hash`, fingerprint de entrada/saída e commit.
- `metrics.jsonl`: uma linha JSON por execução com `prev_run_hash` e `run_hash` encadeado.
- `summary.json`: agregados (`min`, `max`, `mediana`, `p95`, `variância`) e comparação histórica por sessão/perfil.

### Cadeia de custódia

Cada sessão registra:

1. hash do snapshot de entrada (`snapshot_hash`);
2. hash dos artefatos de saída (`output_artifacts`);
3. hash encadeado por execução (`prev_run_hash` -> `run_hash`).

### Opções de CLI / helper / BBS

- `--metrics-store <path>`
- `--append`
- `--new-session`

Exemplo:

```bash
pai bench --repeat 5 --out out_bench --metrics-store rmr/benchmark_framework/output --new-session -- hash --file ./amostra.bin
```
