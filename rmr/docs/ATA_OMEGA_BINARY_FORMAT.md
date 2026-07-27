<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# ATA OMEGA — contrato binário e compatibilidade

## O que foi restaurado

Esta é a referência documental canônica da restauração incorporada ao `master`
no merge commit `e0d08456097ad1a5f1c159b692c7db9925c828f7`.

| Restauração | Estado anterior encontrado | Estado restaurado | Implementação |
| --- | --- | --- | --- |
| `AUTO_ID64` AArch64 | backend atual retornava somente `MPIDR_EL1` | `MIDR_EL1[31:0]` nos 32 bits altos e `MPIDR_EL1[31:0]` nos 32 bits baixos | `rmr/hwif/asm/aarch64/rmr_hwif_backend.S` |
| `HW_SIG64` | aplicava transformação XOR posterior sobre o identificador | recebe diretamente o `AUTO_ID64`, preservando o contrato observado no objeto histórico | `rmr/runtime/rafaelia_core.c` |
| Informações de hardware | logs explícitos haviam sido reduzidos | voltaram `MIDR`, `MPIDR`, `CPU_ID_RAW` e `HW_SIG64` em Base20 | `rmr/runtime/rafaelia_core.c` |
| Cabeçalho ATA V1 | escritor e leitores usavam offsets incompatíveis | escritor canônico de 28 bytes, little-endian | `rmr/runtime/ata_omega_format.h` e `rmr/runtime/rafaelia_core.c` |
| Leitura do binário | leitores esperavam apenas o formato compacto de 12 bytes | detecção e leitura de `V1_EXTENDED` e `LEGACY_COMPACT` | `rmr/runtime/sync_omega.c` e `rmr/runtime/sync_fast.c` |
| Escrita do arquivo | retorno parcial de `write` não era concluído | laço de escrita completa e tratamento de falha | `rmr/runtime/rafaelia_core.c` |
| Segurança de leitura | arquivo truncado podia deslocar campos ou deixar ciclos insuficientes | validação de magic, versão, tamanho, quantidade, sequência, paridade e truncamento | `rmr/runtime/ata_omega_format.h` |
| Decodificação offline | `version` e `record_size` podiam ser interpretados como assinatura | decodificador reconhece offsets corretos nos dois formatos | `rmr/tools/ata_decode.py` |
| Regressão automatizada | não havia prova dedicada do contrato | testes C e Python para V1, compacto, offset e truncamento | `rmr/tests/ata_omega_format_selftest.c` e `rmr/tests/test_ata_omega_format.py` |

### O que não foi restaurado nem reivindicado

- Os arquivos `.o` antigos **não** foram recolocados no Git.
- `HW_SIG64` não é documentado como PUF, serial físico único ou segredo
  criptográfico.
- A existência de outro executável histórico, não localizado, que tenha combinado
  escritor e leitor compatíveis permanece `TOKEN_VAZIO` até aparecer o artefato
  acompanhado de hash.
- Nenhuma alteração foi feita no núcleo criptográfico upstream BLAKE3.

## Diagnóstico histórico

Os objetos AArch64 rastreados no commit
`8ae6b70a672753d6e293b3c84a02b1d5b5ccd824` foram inspecionados diretamente.

- `core.o` contém `AUTO-ID64`, `MIDR`, `MPIDR` e `HW_SIG64`. Sua desmontagem
  mostra a composição `MIDR_EL1[31:0] << 32 | MPIDR_EL1[31:0]` e escritas de
  `4 + 4 + 4 + 4 + 4 + 8` bytes antes dos registros. Portanto, esse objeto já
  produzia o cabeçalho V1 estendido de 28 bytes.
- `sync.o` abre o mesmo arquivo, lê 4 bytes de magic, depois 8 bytes como
  `hw_sig64` e em seguida registros de 24 bytes. Portanto, esse objeto ainda
  consumia o formato compacto de 12 bytes.

Assim, os objetos rastreados não eram um par binariamente coerente entre si,
embora ambos compilassem e preservassem informações esperadas. A existência de
outro executável ligado, em outro commit ou diretório, que tenha usado um par
compatível permanece `TOKEN_VAZIO` sem o respectivo artefato e hash.

A PR #54 removeu corretamente objetos e backups do versionamento. Ela não criou
o defeito; a divergência de contrato já existia antes da remoção.

## Formato canônico V1

Todos os inteiros são unsigned little-endian.

| Offset | Tamanho | Campo | Valor/semântica |
| ---: | ---: | --- | --- |
| 0 | 4 | `magic` | `RFA\0` |
| 4 | 4 | `version` | `1` |
| 8 | 4 | `record_size` | `24` |
| 12 | 4 | `record_count` | `42` |
| 16 | 4 | `reserved` | `0` |
| 20 | 8 | `hw_sig64` | fingerprint HWIF de 64 bits |
| 28 | 1008 | registros | 42 × `{k:u64, cyc:u64, par:u64}` |

Tamanho total V1: `28 + 42 × 24 = 1036 bytes`.

## Formato compacto aceito

Para preservar arquivos compatíveis com o leitor histórico, os leitores também
aceitam:

```text
magic[4] + hw_sig64[8] + 42 * record[24]
```

Tamanho compacto completo: `12 + 42 × 24 = 1020 bytes`.

Os leitores detectam o V1 somente quando `version=1`, `record_size=24`,
`1 <= record_count <= 42` e `reserved=0`. Caso contrário, interpretam o fluxo
como compacto. Registros inválidos, sequência V1 quebrada e arquivos truncados
são rejeitados; não são silenciosamente deslocados.

## Semântica da assinatura

No AArch64, o contrato histórico foi restaurado explicitamente:

```text
AUTO_ID64 = (MIDR_EL1[31:0] << 32) | MPIDR_EL1[31:0]
hw_sig64  = AUTO_ID64
```

No x86_64, `read_cpu_id_raw()` continua sendo o fingerprint determinístico
misturado a partir de registradores CPUID. Em ambos os casos, trata-se de
identificação de modelo/topologia/capacidades observáveis, não de PUF nem de
serial físico globalmente único.

A correção mantém o comportamento histórico do sincronizador: `hw_sig64` é
metadado forense exibido no log; a mistura dinâmica continua baseada em clock,
MIDR, frequência e ciclos ATA. Uma futura vinculação criptográfica deve ser uma
versão de formato separada, não uma alteração silenciosa do V1.

## Implementação

- `rmr/hwif/asm/aarch64/rmr_hwif_backend.S`: restaura `MIDR || MPIDR`;
- `rmr/hwif/include/rmr_hwif.h`: formaliza o contrato por arquitetura;
- `rmr/runtime/ata_omega_format.h`: serialização e parser compartilhados;
- `rmr/runtime/rafaelia_core.c`: escritor V1 canônico e logs MIDR/MPIDR;
- `rmr/runtime/sync_omega.c`: leitor V1 + compacto;
- `rmr/runtime/sync_fast.c`: leitor V1 + compacto e limites seguros;
- `rmr/tools/ata_decode.py`: auditoria offline dos dois formatos;
- `rmr/tests/ata_omega_format_selftest.c`: contrato C V1 + compacto;
- `rmr/tests/test_ata_omega_format.py`: regressão de offset e truncamento.

## Validação

A partir da raiz do repositório:

```sh
cc -std=c11 -Wall -Wextra -Werror \
  rmr/tests/ata_omega_format_selftest.c -o /tmp/ata_omega_selftest
/tmp/ata_omega_selftest
python3 rmr/tests/test_ata_omega_format.py
```

No ambiente AArch64/Termux:

```sh
cd rmr/build
./build_omega.sh
./build_sync.sh
python3 ../tools/ata_decode.py ATA_OMEGA.bin
```

Os binários e objetos continuam sendo artefatos reproduzíveis locais e não devem
ser novamente versionados. A evidência permanente deve ser fonte, teste, log,
hash e recibo de build.
