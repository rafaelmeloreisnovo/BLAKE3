# Documentação do repositório BLAKE3

Esta documentação descreve o conteúdo deste repositório, os principais
componentes, a organização dos diretórios e como executar tarefas
comuns de build/teste. O objetivo é servir como um mapa completo do que
existe aqui.

## Visão geral

O repositório contém a implementação oficial do algoritmo de hash
criptográfico BLAKE3, incluindo:

- A **crate Rust `blake3`**, com implementações otimizadas por SIMD e
  detecção de CPU.
- A **ferramenta CLI `b3sum`**, para calcular hashes BLAKE3 de arquivos
  ou entrada padrão.
- A **implementação em C**, também com otimizações e suporte a
  multithreading.
- Uma **implementação de referência** em Rust, menor e mais simples.
- **Vetores de teste** abrangentes para validar diferentes modos e
  tamanhos de saída.

## Estrutura de diretórios

### Raiz do repositório

- `README.md`: visão geral do projeto, usos e links principais.
- `Cargo.toml` / `Cargo.lock`: manifesto e lockfile do workspace Rust.
- `build.rs`: script de build para configurar a compilação Rust.
- `LICENSE_*`: licenças (CC0, Apache 2.0 e Apache 2.0 com exceções LLVM).
- `CONTRIBUTING.md`: instruções para contribuir.

### `src/` — biblioteca Rust `blake3`

Arquivos principais e módulos internos do crate Rust:

- `lib.rs`: API pública do crate.
- `platform.rs`: seleção dinâmica de implementação conforme CPU.
- `portable.rs`: implementação portável (sem SIMD específico).
- `rust_*` e `ffi_*`: implementações otimizadas por SIMD (SSE2, SSE4.1,
  AVX2, AVX-512, NEON) e bindings internos.
- `guts.rs`, `traits.rs`, `join.rs`, `io.rs`, `hazmat.rs`: camadas
  internas e utilitários para hashing, streaming e APIs avançadas.
- `wasm32_simd.rs`: suporte a SIMD em WASM.
- `test.rs`: testes unitários do crate.

### `b3sum/` — CLI Rust

Diretório do crate `b3sum`, que provê a interface de linha de comando.
Ele utiliza o crate `blake3` internamente e é otimizado para
multithreading.

### `c/` — implementação em C

Implementação oficial em C, com suporte a diversas otimizações:

- `blake3.c`, `blake3.h`: API principal.
- Arquivos `blake3_*` para implementações SIMD específicas (SSE2,
  SSE4.1, AVX2, AVX-512, NEON) e seus correspondentes em assembly.
- `blake3_dispatch.c`: seleção de implementação em runtime.
- `CMakeLists.txt`, `Makefile.testing`: suporte a build e testes.
- `example.c`, `example_tbb.c`: exemplos de uso.
- `README.md`: documentação detalhada da implementação em C.

### `reference_impl/` — implementação de referência

- `reference_impl.rs`: implementação simples e didática.
- `Cargo.toml`/`Cargo.lock`: manifesto próprio da implementação de
  referência.
- `README.md`: explicações e uso.

### `test_vectors/` — vetores de teste

- `test_vectors.json`: conjunto de vetores cobrindo modos e saídas.
- `cross_test.sh`: script para testes cruzados.
- `Cargo.toml`/`Cargo.lock` e `src/`: utilitários Rust para gerar/verificar
  vetores.

### `benches/` — benchmarks

Conjunto de benchmarks para avaliar performance em diferentes cenários.

### `media/`

Imagens e recursos gráficos (ex.: gráficos de desempenho).

### `tools/`

Ferramentas auxiliares do projeto (ex.: scripts de release e detecção
  de suporte a instruções).

## Como compilar e testar

> **Observação:** os comandos abaixo pressupõem que Rust e Cargo estejam
> instalados no sistema.

### Build do crate `blake3`

```bash
cargo build
```

### Testes do crate `blake3`

```bash
cargo test
```

### Build da CLI `b3sum`

```bash
cargo build -p b3sum
```

### Testes da implementação em C

Consulte o arquivo `c/README.md`, que detalha as opções de build e
execução de testes na implementação em C.

## Como usar

### Exemplo rápido em Rust

```rust
let hash = blake3::hash(b"exemplo");
println!("{}", hash);
```

### Exemplo com a CLI `b3sum`

```bash
b3sum arquivo.txt
```

## Licenciamento

O projeto é disponibilizado em domínio público via CC0 1.0, com opções
alternativas sob Apache 2.0 e Apache 2.0 com exceções LLVM.

## Onde encontrar mais detalhes

- `README.md`: visão geral e links de referência.
- `c/README.md`: documentação específica da implementação em C.
- `reference_impl/README.md`: detalhes da implementação de referência.
- `CONTRIBUTING.md`: guia de contribuição.

## Análise de performance e comparação

Esta seção resume onde a documentação de desempenho está concentrada e
como interpretar comparações de performance no contexto do repositório.

### Fontes de benchmarks e gráficos

- `README.md`: contém o gráfico principal (`media/speed.svg`) e links
  para o benchmark exemplificado (16 KiB em Cascade Lake-SP) e para o
  paper do BLAKE3, onde há resultados mais detalhados.
- `media/speed.svg`: gráfico de referência usado na documentação.
- `benches/`: código de benchmarks para avaliar cenários específicos.
- `c/README.md`: descreve particularidades de performance na
  implementação em C, especialmente sobre multithreading.

### Como interpretar a comparação

1. **Hardware importa**: benchmarks variam bastante entre CPUs. As
   comparações oficiais usam uma máquina específica (Cascade Lake-SP).
2. **Tamanho de entrada**: entradas pequenas favorecem overhead menor;
   entradas grandes mostram melhor ganho com paralelismo.
3. **Paralelismo e SIMD**: BLAKE3 aproveita SIMD e múltiplos threads
   automaticamente, o que costuma ampliar a diferença em relação a
   hashes mais antigos.
4. **Comparações reais**: para comparar com outros hashes (SHA-2/3,
   BLAKE2 etc.), prefira medir no seu ambiente e com seus tamanhos de
   arquivo.

### Como medir no seu ambiente

Use a CLI `b3sum` para uma comparação simples contra SHA-256:

```bash
# Crie um arquivo grande (1 GB).
head -c 1000000000 /dev/zero > /tmp/bigfile
# Hash com SHA-256.
time openssl sha256 /tmp/bigfile
# Hash com BLAKE3.
time b3sum /tmp/bigfile
```

Para benchmarks mais detalhados (e repetíveis), utilize os targets em
`benches/` e registre os parâmetros do hardware (CPU, threads,
instruções SIMD disponíveis) para comparação honesta.
