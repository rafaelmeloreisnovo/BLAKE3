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
> - **Camada externa**: conteúdo adicional isolado em `rmr/` e documentos
>   autorais fora de `rmr/` (ex.: `DOCUMENTACAO.md`, `RELATORIO*.md`).
>   Essa camada **não modifica** o núcleo.

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
- Documentos autorais em raiz (como `DOCUMENTACAO.md`, `RELATORIO.md`,
  `RELATORIO_AUDITORIA.md`, `MANIFESTO*.md`, `FORK_NOTES.md`, `AGENTS.md`)
  também compõem a camada externa e seguem licença RMR.

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

## Política de licença no módulo RMR

No módulo `rmr/`, o arquivo `rmr/LICENSE_RMR` contém **somente** o texto legal
da licença. Conteúdos não jurídicos (manifestos, notas conceituais e blocos
técnicos ilustrativos) ficam separados em `rmr/MANIFESTO_RAFAELIA.md`.

## Proveniência e autoria

Este repositório separa explicitamente o upstream BLAKE3 da camada
externa. O mapa de proveniência oficial está em `rmr/PROVENIENCE.md`.
Qualquer novo arquivo autoral deve ser registrado nesse documento com
origem, licença e finalidade. Itens autorais fora de `rmr/` também são
permitidos quando explicitamente catalogados na seção
"Itens fora de `rmr/` sob autoria externa".


### Licenças por fronteira

- **Upstream BLAKE3** (`src/`, `c/`, `b3sum/`, `reference_impl/`, `test_vectors/`,
  `tools/`, `benches/`, `media/` e metadados oficiais):
  **CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions** (conforme `LICENSE_*`).
- **Camada externa autoral** (`rmr/` + documentos autorais fora de `rmr/`):
  **RMR Module License** (`rmr/LICENSE_RMR`).

Esse recorte de fronteira/licença deve permanecer idêntico ao descrito em
`rmr/PROVENIENCE.md`.

As regras para exceções de cabeçalho de licença inline (critérios,
registro obrigatório e fallback documental) estão definidas em
`rmr/ARCHITECTURE.md`, na seção **"Exceções explícitas de cabeçalho inline"**.

## Política de artefatos de build (RMR)

Artefatos de build e arquivos temporários do módulo `rmr/` (por exemplo,
`*.o`, `*.bak` e variantes temporárias) devem ser gerados **apenas localmente**
durante compilação/depuração.

- Esses arquivos **não** fazem parte do código-fonte.
- Esses arquivos **não** devem ser versionados no Git.
- O repositório mantém regras em `.gitignore` para impedir novo versionamento
  acidental desses artefatos.

## Diferenças vs upstream (revisão atual)

- Adicionado `rmr/include/rmr_governance.h` como contrato de governança
  externo ao core.
- Atualizações de documentação em `DOCUMENTACAO.md`, `rmr/ARCHITECTURE.md` e
  material não jurídico segregado em `rmr/MANIFESTO_RAFAELIA.md`.
- Limpeza organizacional no módulo externo `rmr/`: remoção de artefatos de
  build/backup do versionamento e reforço das regras de ignore.

## Matriz de detecção e despacho por arquitetura/SO (RMR)

**Campos de validação objetivos** usados nesta matriz:

- `evidenciado por código`: há implementação/condição explícita em arquivo do repositório.
- `evidenciado por teste`: há teste automatizado explícito cobrindo o ponto.
- `não evidenciado`: não foi encontrado código ou teste que comprove o ponto.

| Arquitetura / SO alvo | Detector | Backend ativo esperado | Fallback | Requisitos de compilação | Riscos conhecidos | Validação (detector) | Validação (backend/fallback) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| x86_64 ou x86_32 (Linux, Windows, Darwin, BSD) | `rmr/detect/detect_x86.c` (`cpuid` + `xgetbv`) | Caminho x86 (flags `RMR_HAS_SSE2`, `RMR_HAS_SSE41`, `RMR_HAS_AVX2`, `RMR_HAS_AVX512` via `rmr/include/rmr_dispatch.h`) | Fallback compile-time (`RMR_COMPILETIME_HAS_*`) quando `RMR_DISABLE_RUNTIME_DETECT` ou sem `caps` | Macros de arquitetura em `rmr/include/rmr_arch.h`; compilador com inline asm x86 para `cpuid/xgetbv` | `AVX2/AVX512` dependem de `OSXSAVE + XCR0`; se o SO não habilitar contexto estendido, extensão não ativa | evidenciado por código | evidenciado por código |
| AArch64 (Linux/Android/Darwin/BSD) | `rmr/detect/detect_aarch64.c` | Caminho ARM (`RMR_HAS_NEON` via `rmr/include/rmr_dispatch.h`) | Sem `RMR_AARCH64_ASSUME_PRIVILEGED`: usa apenas macro compile-time `__ARM_NEON`; com macro desligada mantém `execution_mode=user` | Arquitetura `RMR_ARCH_AARCH64`; para modo privilegiado, build com `RMR_AARCH64_ASSUME_PRIVILEGED` | Leitura de `ID_AA64ISAR0_EL1`/`CurrentEL` via `MRS` pode falhar em userland; modo padrão evita isso | evidenciado por código | evidenciado por código |
| ARM/RISC-V/PPC e demais arquiteturas | `rmr/detect/detect_fallback.c` | Sem backend SIMD dedicado no detector; usa máscara mínima | `simd_extensions=0` (exceto `__ARM_NEON`) e largura de registrador por `sizeof(void*)` | Qualquer alvo não coberto por `RMR_ARCH_X86_*` e `RMR_ARCH_AARCH64` | Cobertura limitada de extensões SIMD (detecção conservadora) | evidenciado por código | evidenciado por código |
| Todas as combinações acima | Cobertura de validação automatizada para matriz de dispatch/detector | N/A | N/A | N/A | Sem suíte dedicada documentada para validar a matriz por arquitetura/SO nesta árvore | não evidenciado | não evidenciado |

### Limitações atuais consolidadas (detector/dispatch)

1. **AArch64 privilegiado vs userland:** o modo detalhado de detecção (`RMR_AARCH64_ASSUME_PRIVILEGED`) depende de acesso a registradores EL1; em userland, o caminho seguro não consulta esses registradores.
2. **Dependência de `XCR0` no x86:** `AVX2` e `AVX512` só são habilitados quando CPU e SO indicam suporte conjunto (`OSXSAVE` + bits necessários em `XCR0`).
3. **Cobertura conservadora fora de x86/AArch64:** `rmr/detect/detect_fallback.c` não faz enumeração avançada de SIMD para RISC-V/PPC/outros.
4. **Detecção de SO Android em `rmr/include/rmr_arch.h`:** a ordem atual verifica `__linux__` antes de `__ANDROID__`; em toolchains onde ambos são definidos, o alvo pode ser classificado como Linux.

### Checklist de atualização da matriz (obrigatório em mudanças de dispatch)

Atualizar esta matriz sempre que houver mudanças em:

- `rmr/detect/`
- `rmr/asm/`
- `rmr/include/rmr_dispatch.h`

Checklist de revisão:

- [ ] Revalidar linhas afetadas de arquitetura/SO na matriz (detector, backend, fallback).
- [ ] Marcar cada campo de validação como `evidenciado por código`, `evidenciado por teste` ou `não evidenciado`.
- [ ] Consolidar novas limitações na seção única **Limitações atuais consolidadas (detector/dispatch)**.
- [ ] Atualizar `rmr/ARCHITECTURE.md` e `rmr/PROVENIENCE.md` se houver impacto de organização/autoria.

## Observação final

Esta árvore **não cria um novo hash** e **não renomeia** o BLAKE3. Ela
apenas redistribui o upstream com uma camada externa isolada e
claramente documentada.
