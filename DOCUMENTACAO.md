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
  - `rmr/benchmark_framework/`: blueprint do framework de benchmark industrial
    (isolado do core).
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
- `rmr/benchmark_framework/`: blueprint do framework de benchmark (RMR).

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

## Benchmarking (camada RMR, sem tocar no core)

O framework de benchmark **não** altera o núcleo do BLAKE3. Ele fica isolado em
`rmr/benchmark_framework/` e deve consumir o BLAKE3 somente como biblioteca ou
CLI externa. O desenho prevê **duas interfaces obrigatórias**:

- **CLI** (`rmr-bench`): execução automática por parâmetros.
- **BBS/TUI**: interface textual interativa (menus/teclado).

Exemplo de uso esperado (não executado nesta documentação):

```bash
rmr-bench --profile ram --size 4GiB --runs 5 --seed 123
rmr-bench --profile io --file ./big.dat --runs 5 --threads 1
rmr-bench --profile pipeline --runs 5 --save report.json
```

Saídas previstas: JSON, CSV e Markdown, com registro de seed, tamanho, threads,
flags, commit e timestamp.

## Proveniência e autoria

Este repositório separa explicitamente o upstream BLAKE3 da camada
externa. O mapa de proveniência oficial está em `rmr/PROVENIENCE.md`.
Qualquer novo arquivo autoral deve ficar em `rmr/` e ser registrado
nesse documento.

## Política de artefatos de build (RMR)

Artefatos de build e arquivos temporários do módulo `rmr/` (por exemplo,
`*.o`, `*.bak` e variantes temporárias) devem ser gerados **apenas localmente**
durante compilação/depuração.

- Esses arquivos **não** fazem parte do código-fonte.
- Esses arquivos **não** devem ser versionados no Git.
- O repositório mantém regras em `.gitignore` para impedir novo versionamento
  acidental desses artefatos.

## Política de cabeçalhos de licença em `rmr/`

Esta política define como declarar copyright/licença em arquivos autorais da
camada externa `rmr/`. As fontes normativas são:

- `rmr/LICENSE_RMR` (texto legal aplicável ao módulo RMR).
- `rmr/PROVENIENCE.md` (classificação upstream vs externo e escopo de autoria).

### Escopo

- Obrigatória para **novos arquivos autorais** dentro de `rmr/`.
- Obrigatória para arquivos existentes em `rmr/` quando forem alterados de forma
  substancial.
- Não se aplica a arquivos upstream fora de `rmr/`.

### Tipos suportados (padrão técnico por extensão)

O cabeçalho deve aparecer no topo do arquivo com um destes formatos:

- `*.c`, `*.h`, `*.rs`, `*.s`, `*.S`, `*.inc`, `*.ld`:

  ```text
  /*
   * Copyright (c) 2025 Rafael
   * License: RMR Module License (see rmr/LICENSE_RMR)
   */
  ```

- `*.sh`, `*.bash`, `*.py`, `*.rb`, `*.pl`, `Makefile`, `*.mk`:

  ```text
  # Copyright (c) 2025 Rafael
  # License: RMR Module License (see rmr/LICENSE_RMR)
  ```

- `*.md`, `*.txt`, `*.yaml`, `*.yml`, `*.toml`, `*.json`:

  ```text
  <!--
  Copyright (c) 2025 Rafael
  License: RMR Module License (see rmr/LICENSE_RMR)
  -->
  ```

### Exceções formais

Exceções só são válidas quando documentadas em `rmr/PROVENIENCE.md` com
justificativa explícita:

1. Arquivos de terceiros importados para `rmr/` com licença própria.
2. Arquivos gerados automaticamente cujo formato quebra com comentário de topo.
3. Casos legais/comerciais onde o cabeçalho precisa de texto adicional aprovado.

### Processo de revisão

- Em cada commit/PR que tocar `rmr/`, verificar se o cabeçalho segue o padrão
  por extensão.
- Confirmar que exceções estão registradas em `rmr/PROVENIENCE.md`.
- Reforçar que `rmr/LICENSE_RMR` e `rmr/PROVENIENCE.md` são a referência
  normativa para dúvidas de classificação e licença.
- Executar verificação estática periódica para detectar ausência/inconsistência
  de cabeçalhos no escopo `rmr/`.

## Diferenças vs upstream (revisão atual)

- Adicionado `rmr/include/rmr_governance.h` como contrato de governança
  externo ao core.
- Atualizações de documentação em `DOCUMENTACAO.md` e `rmr/ARCHITECTURE.md`.
- Limpeza organizacional no módulo externo `rmr/`: remoção de artefatos de
  build/backup do versionamento e reforço das regras de ignore.

## Observação final

Esta árvore **não cria um novo hash** e **não renomeia** o BLAKE3. Ela
apenas redistribui o upstream com uma camada externa isolada e
claramente documentada.
