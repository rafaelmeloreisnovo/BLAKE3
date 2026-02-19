# Arquitetura do módulo RMR (camada externa)

Este documento descreve a arquitetura do módulo **RMR** dentro deste
repositório. O RMR é **externo** ao núcleo BLAKE3 upstream e permanece
isolado para evitar mistura de autoria/licença e para não interferir no
código criptográfico oficial.

## Escopo e isolamento

- **Upstream BLAKE3**: permanece intacto em `src/`, `c/`,
  `reference_impl/`, `b3sum/`, `test_vectors/`, `tools/`, `benches/`,
  `media/` e documentos oficiais.
- **RMR (externo)**: concentrado em `rmr/`, sem integração obrigatória
  com o núcleo BLAKE3.

## Cadeia de execução (casca Java → dispatcher C → backend ASM)

```text
[Java shell / casca externa]
          |
          v
[rmr/rafaelia_core.c]
  bootstrap C + detector runtime
          |
          v
[rmr/rmr_hwif.c]
  seleciona ABI interna de hardware
          |
   +------+------+
   |             |
   v             v
[aarch64 ASM] [x86_64 ASM]
 tempo/cpu_id/raw_write (mesma ABI)
          |
          v
 fallback C determinístico (quando ASM indisponível)
```

## Componentes do RMR

- `rmr/include/`: headers auxiliares do módulo externo
  (ex.: detecção de arquitetura/OS, contrato de governança e `rmr_hwif.h`).
- `rmr/rmr_hwif.c`: detector runtime e seleção de backend.
- `rmr/rafaelia_core.c`: dispatcher C que consome apenas `rmr_hwif`.
- `rmr/asm/aarch64/`: backend ASM AArch64 (ABI `rmr_hwif_*`).
- `rmr/asm/x86_64/`: backend ASM x86_64 (ABI `rmr_hwif_*`).
- `rmr/rust/`: módulos Rust externos **não** integrados ao crate
  `blake3`.
- `rmr/ARCHITECTURE.md`: este documento.
- `rmr/PROVENIENCE.md`: mapa de proveniência e licenças.
- `rmr/LICENSE_RMR`: licença do módulo RMR.

## Diretrizes

1. **Não tocar no núcleo**: o RMR não deve alterar arquivos do upstream.
2. **Integração opcional**: qualquer uso do RMR deve ocorrer fora do
   núcleo (ex.: em ferramentas ou wrappers externos).
3. **Documente mudanças**: arquivos autorais devem ter cabeçalho de
   licença e registro em `rmr/PROVENIENCE.md`.
4. **Sem telemetria**: o módulo não coleta nem transmite dados.

## Observações de uso

O RMR pode conter experimentos de performance ou infraestrutura
auxiliar, mas **não** substitui nem modifica o núcleo BLAKE3. Qualquer
consumidor deve tratá-lo como **camada externa**.


## Detector de capacidades em runtime (`rmr/detect/`)

Para seleção de caminhos SIMD com prioridade em runtime, o RMR agora usa
um detector autoral separado por arquitetura:

- `rmr/detect/detect_x86.c`: leitura direta de CPUID/XGETBV por inline asm
  para mapear `SSE2`, `SSE4.1`, `AVX2` e `AVX512`.
- `rmr/detect/detect_aarch64.c`: leitura de registradores de
  identificação em contexto privilegiado opcional
  (`RMR_AARCH64_ASSUME_PRIVILEGED`) e fallback seguro em userland.
- `rmr/detect/detect_fallback.c`: implementação conservadora para
  arquiteturas sem detector dedicado.
- `rmr/include/rmr_detect.h`: contrato único de saída (`rmr_cpu_caps`).

### Contrato (`rmr_cpu_caps`)

A estrutura `rmr_cpu_caps` expõe os campos normalizados:

1. `architecture`: família da CPU detectada.
2. `simd_extensions`: bitmask de extensões SIMD suportadas.
3. `endianness`: little/big endian.
4. `register_width`: largura do registrador alvo (bits).
5. `execution_mode`: modo de execução observado (user/kernel/hypervisor/baremetal).

### Limitações conhecidas

- Em AArch64 userland, ler registradores EL1 pode causar exceção; por
  isso o modo padrão usa fallback seguro sem `MRS` privilegiado.
- Em x86, `AVX2`/`AVX512` só são marcados quando CPU **e** OS habilitam
  estado estendido em `XCR0`.
- `rmr/include/rmr_dispatch.h` mantém flags compile-time como fallback,
  mas prioriza os bits detectados em runtime por `rmr_get_cpu_caps()`.
