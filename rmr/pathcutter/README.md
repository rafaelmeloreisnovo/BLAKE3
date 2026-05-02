# pathcutter (RMR)

## Objetivo

`rmr/pathcutter/` isola utilidades experimentais de **redução de fricção operacional**
(relativas a criação de diretórios, alocação segura e fail-fast de erros) sem tocar
no núcleo criptográfico upstream do BLAKE3.

## Limites

- Não altera `src/`, `c/`, `reference_impl/` nem backends criptográficos upstream.
- Não expõe dependências externas; usa apenas C padrão/POSIX já adotado no repositório.
- Não contém lógica de hash/cripto; apenas helpers de infraestrutura para a camada RMR.

## Interface com `rmr/core`

O consumo em `rmr/core` permanece estável via `pai_*` em `rmr/core/pai.h`:

- `pai_die`
- `pai_mkdir_p`
- `pai_xmalloc`
- `pai_xfree`

Internamente, `rmr/core/util.c` atua como adaptador para os símbolos deste módulo:

- `rmr_pc_die`
- `rmr_pc_mkdir_p`
- `rmr_pc_xmalloc`
- `rmr_pc_xfree`

## Interface com `rmr/hwif`

Não há ligação direta com `rmr/hwif` hot path. O módulo permanece no plano de suporte
(usuário/runtime), sem chamadas em ASM de backend.
