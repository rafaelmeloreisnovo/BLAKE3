<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# ATA OMEGA — contrato binário e compatibilidade

## Diagnóstico histórico

O objeto AArch64 rastreado antes da limpeza de artefatos continha as mensagens
`AUTO-ID64`, `MIDR`, `MPIDR` e `HW_SIG64`. Ele correspondia ao formato compacto
histórico e funcionava em conjunto com o objeto de sincronização da mesma época.

No commit anterior à remoção dos objetos (`8ae6b70a672753d6e293b3c84a02b1d5b5ccd824`),
a fonte do escritor já emitia um cabeçalho estendido, enquanto as fontes de
`sync_omega` e `ata_decode.py` ainda esperavam o cabeçalho compacto. Portanto, os
objetos rastreados eram builds antigos e autoconsistentes, mas não reproduziam a
fonte presente naquele commit.

A PR #54 removeu corretamente os objetos e backups do versionamento. O defeito
não era a remoção dos binários: era a divergência de contrato escondida pelos
objetos antigos.

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

## Formato histórico aceito

Para preservar os binários e arquivos ATA antigos, os leitores também aceitam:

```text
magic[4] + hw_sig64[8] + 42 * record[24]
```

Tamanho histórico completo: `12 + 42 × 24 = 1020 bytes`.

Os leitores detectam o V1 somente quando `version=1`, `record_size=24`,
`1 <= record_count <= 42` e `reserved=0`. Caso contrário, interpretam o fluxo
como legado. Registros inválidos, sequência V1 quebrada e arquivos truncados são
rejeitados; não são silenciosamente deslocados.

## Semântica da assinatura

`hw_sig64` é um fingerprint determinístico derivado do backend HWIF. Ele registra
capacidades/topologia observáveis da CPU e participa da custódia do arquivo ATA.
Não deve ser descrito como serial físico único ou PUF sem experimento específico.

A correção mantém o comportamento histórico do sincronizador: `hw_sig64` é
metadado forense exibido no log; a mistura dinâmica continua baseada em clock,
MIDR, frequência e ciclos ATA. Uma futura vinculação criptográfica deve ser uma
versão de formato separada, não uma alteração silenciosa do V1.

## Implementação

- `rmr/runtime/ata_omega_format.h`: serialização e parser compartilhados;
- `rmr/runtime/rafaelia_core.c`: escritor V1 canônico com escrita completa;
- `rmr/runtime/sync_omega.c`: leitor V1 + legado;
- `rmr/runtime/sync_fast.c`: leitor V1 + legado e limites seguros;
- `rmr/tools/ata_decode.py`: auditoria offline dos dois formatos;
- `rmr/tests/ata_omega_format_selftest.c`: contrato C V1 + legado;
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
