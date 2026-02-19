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
