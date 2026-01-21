# Documentação do repositório BLAKE3 (fork distribuído)

## Introdução

Este repositório é uma **distribuição de um fork** do projeto **BLAKE3**
upstream oficial. O nome do projeto permanece **BLAKE3**, e o núcleo
criptográfico é tratado como **referência absoluta** do upstream. Este
repositório **não é afiliado** ao time oficial do BLAKE3; ele apenas
redistribui o código e adiciona uma camada externa isolada para fins
locais de organização e experimentação.

> **Resumo de escopo**
> - **Núcleo BLAKE3**: código upstream original (C, ASM, Rust, vetores
>   de teste e documentação oficial).
> - **Camada externa**: conteúdo adicional isolado em `rmr/` e scripts
>   auxiliares. Essa camada **não modifica** o núcleo.

## Camadas e fronteiras

### Núcleo BLAKE3 (upstream)

Componentes que devem permanecer semanticamente idênticos ao upstream:

- `c/` (somente arquivos `blake3*`, `README.md`, `CMakeLists.txt`, etc.).
- `src/` (todos os arquivos Rust do BLAKE3 oficial).
- `reference_impl/` (implementação de referência).
- `test_vectors/` (vetores oficiais e utilitários).
- `README.md` e `LICENSE_*` (documentação e licenças oficiais).
- `b3sum/`, `tools/`, `benches/`, `media/` (conteúdo upstream do projeto).

### Camada externa (autoral / isolada)

Componentes que **não** fazem parte do núcleo BLAKE3 e devem permanecer
separados:

- `rmr/` (documentação, licença e código experimental isolado).
  - `rmr/include/`: headers auxiliares do módulo externo.
  - `rmr/rust/`: módulos Rust externos (não integrados ao crate `blake3`).
- Scripts ou automações específicos (quando existirem) devem evitar
  tocar no núcleo.

> **Regra:** código externo **pode usar** o BLAKE3 como biblioteca, mas
> **nunca** deve modificar ou invadir o núcleo.

## Estrutura de diretórios (visão geral)

- `README.md`: README oficial do BLAKE3 upstream.
- `LICENSE_*`: licenças oficiais do upstream.
- `src/`: crate Rust `blake3` (núcleo upstream).
- `b3sum/`: CLI oficial para hashing.
- `c/`: implementação C oficial (SIMD/ASM/dispatch).
- `reference_impl/`: implementação de referência (Rust).
- `test_vectors/`: vetores e utilitários oficiais.
- `benches/`, `tools/`, `media/`: conteúdo upstream.
- `rmr/`: camada externa isolada (ver `rmr/ARCHITECTURE.md`).

## Build e testes (alinhado ao README oficial)

> Os comandos abaixo seguem o README upstream. Consulte `README.md` e
> `c/README.md` para detalhes completos.

### Rust (crate `blake3` e CLI `b3sum`)

```bash
cargo build
cargo test
cargo build -p b3sum
```

### C (implementação oficial)

Siga as instruções em `c/README.md` para compilar e testar.

## Proveniência e autoria

Este repositório separa explicitamente o upstream BLAKE3 da camada
externa. O mapa de proveniência oficial está em `rmr/PROVENIENCE.md`.
Qualquer novo arquivo autoral deve ficar em `rmr/` e ser registrado
nesse documento.

## Observação final

Esta árvore **não cria um novo hash** e **não renomeia** o BLAKE3. Ela
apenas redistribui o upstream com uma camada externa isolada e
claramente documentada.
