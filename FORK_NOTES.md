# Notas do Fork BLAKE3

## O que mudou em relação ao upstream

Este repositório **redistribui** o BLAKE3 upstream e adiciona uma camada
externa isolada em `rmr/`. O núcleo BLAKE3 permanece como referência
absoluta e não deve ser modificado.

Principais diferenças:

- **Documentação adicional** sobre camadas, proveniência e revisão
  (ex.: `DOCUMENTACAO.md`, `rmr/PROVENIENCE.md`).
- **Camada externa RMR** em `rmr/`, com utilitários e scripts próprios.
- **Artefatos de auditoria** em `audit/` com diffs gerados contra o
  upstream oficial.

## Como buildar no Termux (Android)

> Estes passos são para **consumo local** do fork. O upstream oficial
> continua sendo a referência.

### Dependências básicas

```bash
pkg update
pkg install git clang make cmake python rust
```

### Build do núcleo Rust (crate `blake3` / `b3sum`)

```bash
cargo build
cargo build -p b3sum
```

### Build da implementação C (CMake)

```bash
cmake -S c -B build-termux
cmake --build build-termux
```

### Scripts RMR (externos)

Os scripts abaixo vivem em `rmr/` e são opcionais:

- `rmr/build_termux.sh`: build local específico para Termux.
- `rmr/bench_termux.sh`: benchmarks locais em Termux.

## Observações

- O BLAKE3 upstream permanece **inalterado** no núcleo.
- A camada RMR é **externa** e não deve invadir `c/` ou `src/`.
