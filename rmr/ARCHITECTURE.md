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

## Arquiteturas alvo, ABIs e registradores

Esta seção descreve as arquiteturas suportadas e as convenções de
chamada esperadas para integração C↔ASM. As anotações abaixo refletem
as convenções padrão dos toolchains usados pelo BLAKE3.

### x86-64 (System V, Linux/macOS/BSD)

- **ABI**: System V AMD64.
- **Argumentos**: `RDI`, `RSI`, `RDX`, `RCX`, `R8`, `R9`.
- **Retorno**: `RAX`.
- **Callee-saved**: `RBX`, `RBP`, `R12–R15`.
- **Stack**: alinhamento de 16 bytes na entrada da função.

### x86-64 (Windows x64, MSVC/MinGW)

- **ABI**: Microsoft x64.
- **Argumentos**: `RCX`, `RDX`, `R8`, `R9`.
- **Retorno**: `RAX`.
- **Callee-saved**: `RBX`, `RBP`, `RDI`, `RSI`, `R12–R15`.
- **Stack**: alinhamento de 16 bytes e *shadow space* de 32 bytes.

### x86-32 (SysV/Windows)

- **ABI**: i386 SysV ou stdcall/cdecl no Windows.
- **Argumentos**: via stack.
- **Retorno**: `EAX`.
- **Callee-saved**: `EBX`, `EBP`, `ESI`, `EDI`.
- **Stack**: alinhamento típico de 16 bytes no SysV moderno
  (especialmente quando SSE é usado).

### AArch64 (ARM64, Linux/macOS)

- **ABI**: AAPCS64.
- **Argumentos**: `x0–x7`.
- **Retorno**: `x0`.
- **Callee-saved**: `x19–x28`, `x29` (FP), `x30` (LR), `v8–v15`.
- **Stack**: alinhamento de 16 bytes.

### ARMv7 (ARM32, Linux/Android)

- **ABI**: AAPCS32.
- **Argumentos**: `r0–r3`.
- **Retorno**: `r0`.
- **Callee-saved**: `r4–r11`, `sp`, `lr`.
- **Stack**: alinhamento de 8 bytes no limite de chamada.

### PPC64 (Linux/ELFv2)

- **ABI**: ELFv2 (big/little-endian depende do target).
- **Argumentos**: `r3–r10`.
- **Retorno**: `r3`.
- **Callee-saved**: `r14–r31`.
- **Stack**: alinhamento de 16 bytes.

### RISC-V (RV64/RV32)

- **ABI**: LP64/ILP32.
- **Argumentos**: `a0–a7`.
- **Retorno**: `a0`.
- **Callee-saved**: `s0–s11`.
- **Stack**: alinhamento de 16 bytes.

## ASM por arquitetura (layout do BLAKE3)

Os arquivos ASM seguem a mesma convenção de nomes usada pelo BLAKE3:
`blake3_<feature>_<arch>_<os>.S` (GNU/Clang) ou `.asm` (MSVC). O
dispatcher e o build selecionam o caminho correto via `c/rmr_arch.h`,
`c/rmr_dispatch.h`, `c/CMakeLists.txt` e `c/Makefile.testing`.

- **x86-64 (ASM real)**:
  - Unix: `c/blake3_sse2_x86-64_unix.S`,
    `c/blake3_sse41_x86-64_unix.S`, `c/blake3_avx2_x86-64_unix.S`,
    `c/blake3_avx512_x86-64_unix.S`.
  - Windows (GNU): `c/blake3_sse2_x86-64_windows_gnu.S`,
    `c/blake3_sse41_x86-64_windows_gnu.S`,
    `c/blake3_avx2_x86-64_windows_gnu.S`,
    `c/blake3_avx512_x86-64_windows_gnu.S`.
  - Windows (MSVC): `c/blake3_sse2_x86-64_windows_msvc.asm`,
    `c/blake3_sse41_x86-64_windows_msvc.asm`,
    `c/blake3_avx2_x86-64_windows_msvc.asm`,
    `c/blake3_avx512_x86-64_windows_msvc.asm`.
- **ARM64/ARMv7 (ASM marcador)**:
  - `c/blake3_neon_aarch64_unix.S`
  - `c/blake3_neon_armv7_unix.S`
  Esses arquivos são marcadores de ABI para preservar o layout do
  pipeline; o caminho principal continua usando `c/blake3_neon.c`.

## Pipeline: como compilar por alvo

### CMake (recomendado)

- **x86-64 (Linux/macOS)**:
  ```bash
  cmake -S c -B build-x86_64 -DCMAKE_SYSTEM_PROCESSOR=x86_64
  cmake --build build-x86_64
  ```
- **ARM64 (cross-compile)**:
  ```bash
  cmake -S c -B build-aarch64 \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc
  cmake --build build-aarch64
  ```
- **ARMv7 (cross-compile)**:
  ```bash
  cmake -S c -B build-armv7 \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=armv7 \
    -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc
  cmake --build build-armv7
  ```

### Makefile.testing (rápido/local)

- **x86-64 com ASM**:
  ```bash
  make -C c asm
  ```
- **ARM64/ARMv7 (NEON + ASM marcador)**:
  ```bash
  BLAKE3_USE_NEON=1 make -C c asm
  ```

## Pipeline: como validar registradores

1. **Compile com ASM habilitado** (ex.: `cmake --build build-x86_64` ou
   `make -C c asm`).
2. **Inspecione o prólogo/epílogo** do objeto ASM para confirmar
   preservação de callee-saved e alinhamento de stack:
   ```bash
   objdump -d --no-show-raw-insn build-x86_64/CMakeFiles/blake3.dir/blake3_sse2_x86-64_unix.S.o
   ```
3. **Verifique registradores preservados** conforme a seção de ABI
   (ex.: `RBX/RBP/R12–R15` no SysV, `x19–x28` em AArch64).
4. **Para auditoria adicional**, use `llvm-objdump -d` e confirme que
   o ABI esperado se mantém ao trocar de toolchain.
