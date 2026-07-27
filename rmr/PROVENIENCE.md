<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Proveniência do repositório (BLAKE3 vs RMR)

Este documento cataloga a origem por arquivo/pasta para evitar mistura de
autoria entre o upstream **BLAKE3** e a camada externa **RMR**.

## Regras de isolamento

- **Upstream BLAKE3** permanece com autoria e licenças originais.
- **RMR autoral** permanece isolado em `rmr/`.
- Integrações entre blocos devem ser **mínimas, documentadas e explícitas**.
- O **resultado do hash BLAKE3** e sua semântica criptográfica são atribuídos ao
  upstream; extensões RMR não reivindicam autoria sobre o algoritmo.

## Declaração curta para uso em PR/relatório

Use o texto abaixo quando precisar explicitar copyright/fronteira:

> “Esta mudança é autoral apenas na camada RMR (`rmr/` e docs externos
> catalogados). O resultado criptográfico BLAKE3, vetores e semântica do hash
> permanecem do upstream oficial, sem modificação do núcleo.”

## Mapa de proveniência (por arquivo/pasta)

| Caminho | Proveniência | Licença aplicável |
| --- | --- | --- |
| `src/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `c/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `b3sum/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `reference_impl/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `test_vectors/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `benches/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `tools/` | Upstream BLAKE3, salvo itens externos nominalmente listados abaixo | Licença upstream ou `rmr/LICENSE_RMR`, conforme classificação nominal |
| `media/` | Upstream BLAKE3 | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions (ver `LICENSE_*`) |
| `README.md`, `CONTRIBUTING.md`, `LICENSE_*`, `Cargo.toml`, `Cargo.lock`, `build.rs` | Upstream BLAKE3 ou derivação documental explicitamente delimitada | CC0 1.0 / Apache 2.0 / Apache 2.0 LLVM-exceptions e notices aplicáveis; material externo deve ser identificado |
| `rmr/` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/benchmark_framework/` | RMR autoral (blueprint) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/BugOrAdd/` | RMR autoral (base conceitual remodelável) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/ASYNC_PARALLEL_EXECUTION_MODEL.md` | RMR autoral (arquitetura de escalonamento assíncrono por dependências) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/ASYNC_PARALLEL_BENCHMARK_PROTOCOL.md` | RMR autoral (protocolo reproduzível de benchmark e cadeia de evidência) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/ui/` | RMR autoral (front controller de modos) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/include/rmr_governance.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/hwif/include/rmr_hwif.h`, `rmr/hwif/include/rmr_detect.h`, `rmr/hwif/rmr_hwif.c`, `rmr/hwif/asm/aarch64/`, `rmr/hwif/asm/x86_64/`, `rmr/hwif/detect/detect_x86.c`, `rmr/hwif/detect/detect_aarch64.c`, `rmr/hwif/detect/detect_fallback.c` | RMR autoral (interface HW, detecção runtime e backends ASM) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/core/validate.c`, `rmr/core/pai_validate.h` | RMR autoral (validação determinística de invariantes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/core/lowlevel_freestanding.c` | RMR autoral (estado global da arena nomalloc freestanding) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/` | RMR autoral (registro criptográfico, perfil SHA-256, governança, custódia e testes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/ZIP_BITSTACK_CUSTODY_PROFILE.md` | RMR autoral (perfil de cápsula ZIPRAF/RVC1, palavra, empilhamento estrutural, CRC, digests e âncoras) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/registry/architectures.json` | RMR autoral (snapshot de referências públicas; nenhum código de terceiro incorporado) | RMR Module License (`rmr/LICENSE_RMR`) para a seleção/organização autoral; nomes e direitos de terceiros permanecem de seus titulares |
| `rmr/crypto/registry/zip_custody_profile.json` | RMR autoral (snapshot executável das camadas de custódia ZIPRAF/RVC1) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/schemas/architecture-registry.schema.json` | RMR autoral (schema de dados) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/claims/claims.jsonl` | RMR autoral (ledger epistemológico) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/claims/zip_bitstack_claims.jsonl` | RMR autoral (ledger de claims e falsificadores ZIP/CRC/bit-stacking) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/tools/`, `rmr/crypto/tests/` | RMR autoral (auditoria offline e testes de contrato) | RMR Module License (`rmr/LICENSE_RMR`) |
| `.github/workflows/rmr-zip-custody.yml` | Externo autoral RMR (gate CI do perfil ZIPRAF/RVC1) | RMR Module License (`rmr/LICENSE_RMR`) |
| `DOCUMENTACAO.md`, `MANIFESTO*.md` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/MANIFESTO_RAFAELIA.md` | RMR autoral (texto não jurídico) | RMR Module License (`rmr/LICENSE_RMR`) |

## Itens fora de `rmr/` sob autoria externa

Os itens abaixo são **externos ao upstream** e ficam fora de `rmr/` por decisão
organizacional. Eles devem manter rastreabilidade de licença e finalidade:

| Caminho | Origem | Licença aplicável | Finalidade |
| --- | --- | --- | --- |
| `DOCUMENTACAO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Fronteiras de escopo, operação e integração |
| `MANIFESTO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Manifesto e diretrizes de projeto |
| `MANIFESTO_RAFAELIA.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Manifesto complementar/autoral |
| `FORK_NOTES.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Notas de distribuição do fork |
| `RELATORIO.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Relatório técnico consolidado |
| `RELATORIO_AUDITORIA.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Relatório de auditoria e conformidade |
| `AGENTS.md` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Governança operacional para agentes |
| `tools/check_rmr_headers.py` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Verificação de cabeçalho `LICENSE_RMR`, shebang e escopo de alterações |
| `tools/check_rmr_headers.sh` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Wrapper para execução do verificador Python |
| `.github/workflows/rmr-zip-custody.yml` | Externo autoral RMR | RMR Module License (`rmr/LICENSE_RMR`) | Gate CI para KATs, perfil, claims, mutações adversariais e cabeçalhos |
| `rmr/tools/audit_freestanding_nomalloc.py` | Externo autoral | RMR Module License (`rmr/LICENSE_RMR`) | Auditoria estática de coerência do perfil bare-metal `RMR_FREESTANDING_NOMALLOC` |

## Observações sobre integração

- O RMR é **externo** e não integra o núcleo BLAKE3 por padrão.
- Qualquer novo arquivo autoral do RMR **deve**:
  1. Ficar dentro de `rmr/`, ou ser uma integração externa nominalmente registrada nesta tabela.
  2. Incluir cabeçalho de copyright/licença no próprio arquivo ou exceção formal.
  3. Ser registrado neste documento.
  4. Ser isolado do core (sem modificar `src/` ou `c/`).

## Exceções formais de cabeçalho — formatos JSON/JSONL

Os seguintes arquivos não aceitam comentários de topo sem invalidar seu formato:

- `rmr/crypto/registry/architectures.json`;
- `rmr/crypto/registry/zip_custody_profile.json`;
- `rmr/crypto/schemas/architecture-registry.schema.json`;
- `rmr/crypto/claims/claims.jsonl`;
- `rmr/crypto/claims/zip_bitstack_claims.jsonl`.

Justificativa técnica: JSON e JSONL estritos não possuem sintaxe de comentário. A autoria e a licença são codificadas como dados (`_meta`, `$comment` ou primeiro registro `meta`) e esta exceção é registrada em 2026-07-26 conforme os critérios de `rmr/docs/ARCHITECTURE.md`.

Compliance Notice

All licensing and provenance decisions in this repository
are manually reviewed by the author.

No automated tool or AI system is authorized to alter
licensing terms, introduce legal assumptions, or publish
code on behalf of the author.

## Separação entre conteúdo jurídico e não jurídico

- `rmr/LICENSE_RMR` deve conter apenas o texto legal da licença RMR.
- Conteúdos técnicos/conceituais não jurídicos ficam em
  `rmr/MANIFESTO_RAFAELIA.md` ou em documentação técnica nominal.

## Nota de auditoria (benchmark telemetry store)

Mudanças de telemetria/armazenamento de benchmark permanecem restritas a `rmr/` (ex.: `rmr/core/bench.c`, `rmr/benchmark_framework/README.md`, `rmr/ui/mode_router.c`) e não alteram o núcleo criptográfico upstream (`src/`, `c/`, `reference_impl/`).

### Atualização 2026-05-02 (governança de telemetria)

Extensões de governança/telemetria foram aplicadas apenas em `rmr/include/rmr_governance.h` e `rmr/core/bench.c`, mantendo isolamento no módulo RMR e sem alterações no núcleo BLAKE3 upstream (`src/`, `c/`, `reference_impl/`).

### Atualização 2026-05-02 (validação RMR)

Adicionado comando `pai validate` (arquivos `rmr/core/validate.c` e `rmr/core/pai_validate.h`) para validar invariantes matemáticos/operacionais do módulo RMR em runtime, mantendo isolamento da camada externa e sem alterar o núcleo BLAKE3 upstream.

### Fronteira explícita (auditoria): upstream vs externo

- **Upstream (imutável no núcleo criptográfico)**: `src/`, `c/`, `reference_impl/`, `b3sum/`, `test_vectors/`, `tools/`, `benches/`, `media/` e metadados oficiais, ressalvadas derivações documentais e ferramentas externas nominalmente registradas.
- **Externo autoral (isolado)**: `rmr/` e documentos autorais fora de `rmr/` listados neste arquivo.
- **Regra operacional**: mudanças em detecção/dispatch do RMR devem permanecer no domínio externo (`rmr/*`) e nunca alterar a lógica criptográfica upstream.

### Atualização 2026-05-02 (trilha `pathcutter`)

Criada trilha autoral externa `rmr/pathcutter/` para utilidades experimentais de redução de fricção operacional (mkdir recursivo, alocação segura e fail-fast), com interface estável consumida por `rmr/core/util.c` e sem qualquer alteração no núcleo BLAKE3 upstream (`src/`, `c/`, `reference_impl/`).

Também foi adicionado o script de auditoria estática `rmr/tools/audit_pathcutter_static.py`, restrito ao módulo externo `rmr/pathcutter/`.

### Atualização 2026-05-24 (freestanding `nomalloc`)

Adicionado modo autoral externo de alocação estática para ambientes
`RMR_NO_LIBC` com `RMR_FREESTANDING_NOMALLOC`, implementado em
`rmr/include/rmr_lowlevel.h` via arena linear configurável por
`RMR_FREESTANDING_ARENA_SIZE` e reset explícito por
`rmr_ll_freestanding_reset_allocator()`.

A mudança é estritamente de infraestrutura externa em `rmr/` e não altera
qualquer lógica criptográfica do núcleo BLAKE3 upstream (`src/`, `c/`,
`reference_impl/`).

### Atualização 2026-07-21 (paralelismo assíncrono orientado por dependências)

Registrados os documentos autorais externos:

- `rmr/docs/ASYNC_PARALLEL_EXECUTION_MODEL.md`;
- `rmr/docs/ASYNC_PARALLEL_BENCHMARK_PROTOCOL.md`.

Os documentos distinguem AVX-512, multicore e TBB; especificam um DAG sem
barreira global por nível, com joins locais determinísticos; e definem o
protocolo de medição necessário para promover resultados de `TOKEN_VAZIO` para
evidência reproduzível. Nenhum arquivo do núcleo BLAKE3 foi alterado.

### Atualização 2026-07-26 (RMR Crypto Registry)

Criado `rmr/crypto/` como módulo autoral externo para catalogar dez famílias de implementações criptográficas e trinta candidatos relacionados, documentar o perfil SHA-256 já exposto pelo RMR e fornecer auditoria offline de estrutura, claims e vetores conhecidos.

A inclusão no catálogo é somente referência. Não houve importação de código de terceiro. Licença e parentesco de fork permanecem `TOKEN_VAZIO` quando não foram verificados em commit fixado. O texto de `rmr/LICENSE_RMR` não foi alterado. A branch/PR deve permanecer sujeita a revisão humana antes de publicação ou merge.

### Atualização 2026-07-26 (custódia ZIPRAF/RVC1 e empilhamento estrutural)

Registrados os artefatos autorais externos:

- `rmr/crypto/ZIP_BITSTACK_CUSTODY_PROFILE.md`;
- `rmr/crypto/registry/zip_custody_profile.json`;
- `rmr/crypto/claims/zip_bitstack_claims.jsonl`;
- `rmr/crypto/tools/validate_zip_custody_profile.py`;
- `rmr/crypto/tests/test_zip_custody_profile.py`;
- `.github/workflows/rmr-zip-custody.yml`.

O perfil fixa a evidência já existente de serialização RVC1, vínculo público `CRC32C(label) -> class_id`, cápsula CRC32C, ZIP method 0 `STORE`, roundtrip byte a byte e SHA-256 do payload. Ele separa codificação estrutural, detecção de erro, digest criptográfico, autenticação e confidencialidade; não altera `rmr/LICENSE_RMR`, o núcleo BLAKE3 upstream ou os arquivos `src/`, `c/` e `reference_impl/`. Assinatura, timestamp/DOI externo, BLAKE3 do arquivo e modo com chave permanecem `TOKEN_VAZIO` até evidência própria e revisão humana. O workflow executa somente validação de contrato, KATs, testes adversariais e verificação de cabeçalhos.
