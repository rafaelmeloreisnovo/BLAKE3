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

## Proveniência e autoria (BLAKE3 vs RMR)

Esta árvore contém o **BLAKE3** (upstream) e extensões do módulo
**RMR** (Rafael Melo Reis). Para evitar confusão de autoria e manter
conformidade de licença, a separação operacional é a seguinte:

### Núcleo BLAKE3 (upstream)

Componentes que seguem o projeto original, com licenças CC0/Apache (e
variações conforme os arquivos):

- `src/` (exceto `src/rmr.rs`): API principal e IO padrão.
- `c/` (exceto `c/rmr_lowlevel.h`): implementação C oficial.
- `reference_impl/`: implementação de referência.
- `b3sum/`, `tools/`, `media/`: utilitários e documentação upstream.

### Extensões RMR (autoria do módulo)

Componentes autorais do módulo RMR, isolados para evitar mistura de
proveniência:

- `src/rmr.rs`: núcleo de configuração do módulo RMR.
- `c/rmr_lowlevel.h`: helpers low-level e modo sem libc para integração
  bare-metal.
- `c/rmr_arch.h`: detecção de arquitetura/OS/compilador para seleção
  low-level e caminhos de assembly.
- `rmr/`: documentação e licença específica do módulo.

### Integração entre os blocos

Integrações deliberadas e explícitas entre upstream e RMR:

- `src/io.rs` consome `crate::rmr::IO_READ_BUF_LEN` para o tamanho de
  buffer de leitura.
- `c/blake3.c` e `c/main.c` usam macros/asserções definidas em
  `c/rmr_lowlevel.h` para rotinas low-level e validações.
- `c/rmr_lowlevel.h` inclui `c/rmr_arch.h` para consolidar detecção de
  plataforma e preparar caminhos específicos por hardware.

Essa separação serve como trilha de auditoria: o que é upstream mantém
os créditos originais, e o que é RMR permanece delimitado no módulo.

## Diretrizes low-level (RMR) e integrações bare-metal

Esta seção descreve decisões de baixo nível voltadas a reduzir
latência, overhead e dependências externas, com foco em integração com
hardware e sistemas operacionais distintos.

### RMR: escopo, isolamento e parâmetros de ajuste

O módulo **RMR** concentra parâmetros e helpers experimentais de
performance para evitar espalhar *knobs* e decisões de micro-otimização
por todo o crate. O objetivo é facilitar ajustes controlados sem
desestabilizar a API principal.

- **Buffer de leitura para IO amplo**: o tamanho `IO_READ_BUF_LEN`
  (no RMR) centraliza um parâmetro que afeta throughput e número de
  syscalls. Ajustes nesse valor devem ser avaliados com *benchmarks* de
  streaming e tamanhos reais de arquivo, observando efeito sobre
  *cache hits* e latência de leitura.

### No-libc e bare-metal

Para cenários **bare-metal** ou com runtime mínimo, o header
`c/rmr_lowlevel.h` oferece um modo sem libc (`RMR_NO_LIBC`), permitindo
fornecer implementações externas para funções básicas de string,
alocação e parsing:

- `rmr_ll_strlen`, `rmr_ll_strcmp`, `rmr_ll_strerror`
- `rmr_ll_malloc`, `rmr_ll_free`
- `rmr_ll_strtoull`

Esse caminho permite integrar o BLAKE3 em ambientes sem `malloc` ou
`strtoull` do sistema, preservando a compatibilidade com fluxos
existentes e mantendo a semântica de validação de números.

### Prefetch, alinhamento e hot loops

Para cópias grandes e alinhadas, o `rmr_memcpy` expõe *prefetch hints*
para compiladores que suportam `__builtin_prefetch`. Esses hints são
aplicados com distância e *stride* definidos como `size_t` para manter
coerência de tipo e permitir ajustes controlados de cache:

- **Distância de prefetch**: antecipa leituras/escritas para linhas de
  cache futuras, reduzindo *stall* de memória.
- **Stride de prefetch**: regula a cadência dos hints para evitar
  saturar o *front-end* ou poluir caches L1/L2.

Em ambientes sem suporte a prefetch (ou com compiladores não
compatíveis), os macros degradam para *no-op*, mantendo comportamento
funcional.

### Recomendações de medição (estilo científico)

Para validar ganhos reais, recomenda-se:

1. **Microbenchmarks isolados**: medir `rmr_memcpy` e `copy_wide` com
   buffers frios/quentes, variando tamanhos e alinhamento.
2. **Perf/PMU**: inspecionar *cache misses*, *branch mispredicts* e
   stalls de memória para confirmar impacto das mudanças.
3. **A/B em hardware real**: CPUs distintas reagem de forma diferente
   a prefetch/stride. Validar em alvos representativos do deploy.

## Análise de performance e comparação

Esta seção resume onde a documentação de desempenho está concentrada e
como interpretar comparações de performance no contexto do repositório.
Ela também traz um guia prático para melhorar velocidade, bandwidth,
throughput e footprint em medições reais (sem prometer ganhos fixos).

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

### Checklist de otimização (velocidade, bandwidth e throughput)

Use os itens abaixo para perseguir ganhos agressivos (às vezes da ordem
de múltiplos *x*, dependendo do hardware, carga e tamanho de entrada):

1. **Garanta SIMD e CPU features habilitadas**: o código seleciona
   automaticamente SSE2/SSE4.1/AVX2/AVX-512/NEON quando disponíveis.
2. **Aumente tamanho de lote**: entradas maiores reduzem overhead e
   aumentam throughput; compare com tamanhos representativos do seu uso.
3. **Avalie multithreading**: para arquivos grandes, múltiplos threads
   ampliam o throughput; para pequenos, podem piorar o tempo total.
4. **Evite cópias desnecessárias**: leituras em buffer e streaming
   estável melhoram a velocidade observada.
5. **Meça com afinidade e carga controlada**: CPU throttle e ruído de
   sistema distorcem resultados (especialmente para metas “17x melhor”).
6. **Compare apples-to-apples**: mesmos parâmetros, I/O equivalente e
   ferramentas configuradas de forma consistente.

### Acuracidade, footprint e outras operações

- **Acuracidade de medição**: repita testes, descarte outliers e
  registre médias/percentis.
- **Footprint**: use entradas de streaming e buffers dimensionados para
  reduzir memória pico.
- **Operações adicionais**: ao medir KDF/MAC/XOF, registre tamanho de
  saída e formato, pois isso influencia throughput.

### Refatoração e upgrades orientados a performance

Se você pretende evoluir o código localmente, concentre melhorias nos
seguintes pontos antes de alterar algoritmos:

1. **Fluxo de I/O**: priorize caminhos que evitam cópias e reutilizem
   buffers (por exemplo, streaming e leitura incremental).
2. **Granularidade de chunk**: alinhe tamanhos de blocos com o pipeline
   de SIMD e com o paralelismo disponível.
3. **Seleção de plataforma**: mantenha a lógica de dispatch enxuta para
   reduzir overhead na escolha de implementação.
4. **Multithreading controlado**: ajuste número de threads e limiares
   para não degradar workloads pequenos.
5. **Benchmarks antes/depois**: valide qualquer upgrade com o mesmo
   conjunto de testes e parâmetros.

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

Para automatizar esse comparativo, use o script
`tools/benchmark_example.sh` (aceita caminho e tamanho opcional):

```bash
./tools/benchmark_example.sh /tmp/bigfile 1000000000
```

Para benchmarks mais detalhados (e repetíveis), utilize os targets em
`benches/` e registre os parâmetros do hardware (CPU, threads,
instruções SIMD disponíveis) para comparação honesta.

### Registro mínimo para comparações confiáveis

Ao publicar ou comparar resultados, registre pelo menos:

- **CPU e frequência** (modelo, número de núcleos e turbo).
- **Sistema operacional** e versão.
- **Compilador e flags** (por exemplo, release vs debug).
- **Tamanho do input** e **modo** (hash, KDF, MAC, XOF).
- **Número de threads** e **features SIMD** ativas.
- **Métrica reportada** (MB/s, tempo total, p50/p95).

### Checklist operacional (passo a passo)

1. Defina o cenário (input, modo, tamanho de saída).
2. Execute 5–10 repetições e registre média/percentis.
3. Compare com outra função de hash sob o mesmo I/O.
4. Ajuste parâmetros (threads, buffer, chunk) e repita.
5. Documente o ganho e o custo (memória/CPU).

### Escopo de mudanças no repositório

Para evitar alterações desnecessárias, priorize mudanças pontuais nos
arquivos que realmente influenciam o caminho crítico de hashing:

- **Rust (`src/`)**: otimizações e ajustes de desempenho.
- **C (`c/`)**: melhorias de SIMD/dispatch e multithreading.
- **`benches/`**: novos cenários de benchmark.

Evite “codificar em todos os arquivos” sem necessidade. Mudanças
cirúrgicas tornam o impacto mensurável e reduzem regressões.
