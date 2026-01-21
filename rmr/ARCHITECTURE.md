# Arquitetura do módulo RMR

Este documento descreve a arquitetura inicial do módulo **RMR** dentro
do repositório BLAKE3. O objetivo é oferecer um espaço isolado para
evoluções focadas em performance, sem impactar o núcleo do hash.

## Proveniência e autoria

O módulo **RMR** (Rafael Melo Reis) é autoral e deve permanecer isolado
para garantir separação clara em relação ao núcleo **BLAKE3** upstream.
Essa separação evita confusão de licença e facilita auditoria:

- **Upstream BLAKE3**: todo o conteúdo de `src/` exceto `src/rmr.rs`,
  além de `c/` (exceto `c/rmr_lowlevel.h`), `reference_impl/`, `b3sum/`
  e `tools/`.
- **RMR**: `src/rmr.rs`, `c/rmr_lowlevel.h`, `c/rmr_arch.h` e `rmr/`
  (documentação e licença do módulo).

## Objetivos

- **Isolamento**: manter mudanças experimentais contidas em `src/rmr.rs`.
- **Evolução incremental**: permitir upgrades graduais com benchmarks
  antes/depois.
- **Integração controlada**: expor apenas uma interface mínima para
  evitar regressões.

## Componentes

- `src/rmr.rs`: ponto central do módulo, com configuração e versão.
- `rmr/ARCHITECTURE.md`: documentação de arquitetura (este arquivo).
- `rmr/LICENSE_RMR`: licença específica do módulo RMR.
- `c/rmr_arch.h`: detecção de arquitetura/OS/compilador para seleção
  low-level controlada.

## Diretrizes

1. Qualquer ajuste deve ser acompanhado de testes e benchmarks.
2. Evite dependências adicionais sem necessidade clara.
3. Prefira alterações pequenas e mensuráveis.
4. Use `c/rmr_arch.h` como ponto único para expandir seleção de
   arquitetura e caminhos de assembly.

## Mapa de seleção por arquitetura/OS/compilador

O ponto central de seleção agora é `c/rmr_dispatch.h`, que deriva flags
de build a partir de `c/rmr_arch.h` e das macros do compilador. Ele
expõe `RMR_HAS_*` (ex.: `RMR_HAS_AVX2`, `RMR_HAS_NEON`) e preserva os
`RMR_OS_*`/`RMR_ARCH_*` para orientar o build e o código C. Essa camada
serve como referência única para decidir quais backends devem entrar no
binário.

### Seleção de ASM (x86-64)

- **Windows + MSVC**: usa `c/blake3_*_x86-64_windows_msvc.asm` via CMake.
- **Windows + GCC/Clang (MinGW/Cygwin)**: usa `c/blake3_*_x86-64_windows_gnu.S`.
- **Linux/macOS/BSD + GCC/Clang**: usa `c/blake3_*_x86-64_unix.S`.

Essas escolhas são controladas no `c/CMakeLists.txt` e no
`c/Makefile.testing` com base nos flags `RMR_ARCH_X86_64` e `RMR_OS_*`.

### Seleção de C intrinsics

- **x86 32-bit**: usa `c/blake3_sse2.c`, `c/blake3_sse41.c`,
  `c/blake3_avx2.c`, `c/blake3_avx512.c` com flags do compilador
  (`-msse2`, `-msse4.1`, `-mavx2`, `-mavx512f -mavx512vl` ou `/arch:*`).
- **ARM/AArch64**: usa `c/blake3_neon.c` com `BLAKE3_USE_NEON=1` e,
  quando necessário, `-mfpu=neon`.

### Código portable e dispatcher

- **C portable**: `c/blake3_portable.c` sempre disponível.
- **Dispatcher**: `c/blake3_dispatch.c` escolhe em runtime o backend
  SIMD, enquanto `c/rmr_dispatch.h` expõe a matriz de compatibilidade
  em tempo de compilação.
