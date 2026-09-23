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
| `rmr/benchmark_framework/` | RMR autoral (benchmark/reproducibility framework; blueprint + executable BLAKE3 harness) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/CMakeLists.txt`, `rmr/core/hash_blake3.c` | RMR autoral (build/adaptador externo que consome a API pública do BLAKE3 em `c/`) | RMR Module License (`rmr/LICENSE_RMR`) para a camada RMR; BLAKE3 mantém suas licenças upstream |
| `rmr/tools/orchestrate_blake3_compare.sh` | RMR autoral (orquestração reproduzível fork × upstream oficial fixado por SHA) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/BugOrAdd/` | RMR autoral (base conceitual remodelável) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/ASYNC_PARALLEL_EXECUTION_MODEL.md` | RMR autoral (arquitetura de escalonamento assíncrono por dependências) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/docs/ASYNC_PARALLEL_BENCHMARK_PROTOCOL.md` | RMR autoral (protocolo reproduzível de benchmark e cadeia de evidência) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/ui/` | RMR autoral (front controller de modos) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/include/rmr_governance.h` | RMR autoral | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/hwif/include/rmr_hwif.h`, `rmr/hwif/include/rmr_detect.h`, `rmr/hwif/rmr_hwif.c`, `rmr/hwif/asm/aarch64/`, `rmr/hwif/asm/x86_64/`, `rmr/hwif/detect/detect_x86.c`, `rmr/hwif/detect/detect_aarch64.c`, `rmr/hwif/detect/detect_fallback.c` | RMR autoral (interface HW, detecção runtime e backends ASM) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/core/validate.c`, `rmr/core/pai_validate.h` | RMR autoral (validação determinística de invariantes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/core/lowlevel_freestanding.c` | RMR autoral (estado global da arena nomalloc freestanding) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/topology/`, `rmr/tools/rmr_topology_audit.py`, `rmr/docs/RMR_HARDWARE_BUILD_TOPOLOGY_V1.md` | RMR autoral (topologia tipada de preprocessor/compiler/linker/binário/símbolos/pointers/loops/I/O/comentários/warnings/módulos/overlap) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/pai42/` | RMR autoral (ponte geométrica determinística entre 42 ciclos ATA OMEGA, projeção circular Q16, ferramenta de auditoria e testes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/` | RMR autoral (registro criptográfico, perfil SHA-256, governança, custódia e testes) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/ZIP_BITSTACK_CUSTODY_PROFILE.md` | RMR autoral (perfil de cápsula ZIPRAF/RVC1, palavra, empilhamento estrutural, CRC, digests e âncoras) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/registry/architectures.json` | RMR autoral (snapshot de referências públicas; nenhum código de terceiro incorporado) | RMR Module License (`rmr/LICENSE_RMR`) para a seleção/organização autoral; nomes e direitos de terceiros permanecem de seus titulares |
| `rmr/crypto/registry/zip_custody_profile.json` | RMR autoral (snapshot executável das camadas de custódia ZIPRAF/RVC1) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/schemas/architecture-registry.schema.json` | RMR autoral (schema de dados) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/claims/claims.jsonl` | RMR autoral (ledger epistemológico) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/claims/zip_bitstack_claims.jsonl` | RMR autoral (ledger de claims e falsificadores ZIP/CRC/bit-stacking) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/FORENSIC_TIME_PROFILE.md` | RMR autoral (UTC canônico, contexto civil local e atestação temporal fail-closed) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/registry/forensic_time_profile.jsonl` | RMR autoral (perfil temporal executável) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/claims/forensic_time_claims.jsonl` | RMR autoral (claims/falsificadores do perfil temporal) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/tools/forensic_time_attest.py`, `rmr/crypto/tests/test_forensic_time_attest.py` | RMR autoral (canonização UTC/local, selos temporais e testes adversariais) | RMR Module License (`rmr/LICENSE_RMR`) |
| `rmr/crypto/tools/`, `rmr/crypto/tests/` | RMR autoral (auditoria offline e testes de contrato) | RMR Module License (`rmr/LICENSE_RMR`) |
| `.github/workflows/rmr-zip-custody.yml` | Externo autoral RMR (gate CI do perfil ZIPRAF/RVC1) | RMR Module License (`rmr/LICENSE_RMR`) |
| `.github/workflows/rmr-blake3-repro.yml` | Externo autoral RMR (gate CI do adapter, KAT, benchmark e receipts) | RMR Module License (`rmr/LICENSE_RMR`) |
| `.github/workflows/rmr-hardware-build-topology.yml` | Externo autoral RMR (gate CI da topologia de build/hardware e receipt JSON) | RMR Module License (`rmr/LICENSE_RMR`) |
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
- `rmr/crypto/claims/zip_bitstack_claims.jsonl`;
- `rmr/crypto/registry/forensic_time_profile.jsonl`;
- `rmr/crypto/claims/forensic_time_claims.jsonl`;
- `rmr/pai42/schema/pai42-observation.schema.json`.

Justificativa técnica: JSON e JSONL estritos não possuem sintaxe de comentário. A autoria e a licença são codificadas como dados (`_meta`, `$comment`, `x-rmr-meta` ou primeiro registro `meta`) e esta exceção é registrada conforme os critérios de `rmr/docs/ARCHITECTURE.md`.

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

### Atualização 2026-07-27 (ponte geométrica RMR PAI42)

Criado `rmr/pai42/` para formalizar a ligação entre os 42 registros do `ATA_OMEGA.bin` e uma projeção circular determinística. O núcleo C usa tamanho fixo, aritmética Q16, tabela de 42 direções, zero heap e zero ponto flutuante; a referência Python é `stdlib-only` e permanece fora do hot path.

A estrutura inclui contrato matemático, API C, implementação, schema JSON, leitor dos formatos ATA V1/compacto e testes de regressão. Ela não modifica o núcleo BLAKE3, não declara `HW_SIG64` como PUF e mantém classificação semântica, integração SIMD/NEON e vínculo criptográfico como `TOKEN_VAZIO` até evidência própria.


### Atualização 2026-09-13 (UTC canônico e atestação temporal local)

Criado `RMR-FORENSIC-TIME-V1` na camada externa RMR. O perfil fixa UTC
RFC3339 com `Z` e Unix epoch seconds como identidade temporal canônica,
preserva horário civil local somente com offset explícito e mantém timezone/
horário de verão como contexto forense.

O `instant_digest` é independente da política civil local; o
`local_attestation_digest` inclui offset, timezone declarado, fonte e estado
de sincronização do relógio. Uma preimage separada é produzida para assinatura
destacada futura. Assinatura digital e timestamp externo confiável permanecem
`TOKEN_VAZIO` até evidência própria.

O core `rmr/freestanding_custody16` não passa a ler relógio: a integração é
sidecar vinculada ao digest da evidência. Nenhum arquivo do núcleo BLAKE3
upstream (`src/`, `c/`, `reference_impl/`) foi alterado.


### Atualização 2026-09-21 (adapter BLAKE3 + CMake + benchmark reproduzível)

Foi materializado um adapter explícito em rmr/ que consome a API pública da implementação C BLAKE3 já presente em c/, sem copiar, renomear ou alterar a função criptográfica.

A trilha adiciona:
- CMake próprio do RMR;
- pai hash --algo blake3;
- pai scan --hash blake3;
- harness comum para comparação;
- orquestrador que fixa o upstream oficial por SHA, executa KAT antes de medir e gera receipts SHA-256/BLAKE3;
- gate CI dedicado.

Estados:
- RMR_BLAKE3_ADAPTER = IMPLEMENTED_PENDING_CI
- RMR_CMAKE = IMPLEMENTED_PENDING_CI
- UPSTREAM_COMPARISON_HARNESS = IMPLEMENTED_PENDING_CI
- AUTOMATIC_RECEIPTS = IMPLEMENTED_PENDING_CI
- INDEPENDENT_THIRD_PARTY_REPRODUCTION = TOKEN_VAZIO

Nenhum claim de superioridade de desempenho é promovido pela existência do harness.


### Atualização 2026-09-23 (hardware/build topology V1)

Criado `rmr/topology/` como micro-módulo autoral externo para separar e
observar quinze superfícies: preprocessor, compiler, linker, binary, symbol,
pointer, loop, I/O, comment, warning, module, overlap, condition, color e IOPS.

O módulo não altera a primitiva BLAKE3. `void` é tratado como fronteira
genérica explícita, não como ausência semântica; símbolos lexicais permanecem
candidatos até evidência do linker; contadores de I/O são operações lógicas e
não promovem claim de IOPS físico.

O auditor `rmr/tools/rmr_topology_audit.py` é stdlib-only e usa ferramentas
locais (`cc`, `readelf`, `nm`, `size`) somente quando disponíveis. O
workflow `.github/workflows/rmr-hardware-build-topology.yml` materializa o
gate reproduzível. Estado remoto permanece NOT_RUN até observação de CI.


### Atualização 2026-09-23 (ARMv7 HWIF split + IOPS protocol)

A camada RMR separa ARMv7 user-mode de privileged-mode. O caminho user não
acessa CP15; o caminho privileged exige opt-in explícito
`RMR_ARMV7_ASSUME_PRIVILEGED=1`. A largura de `rmr_u64/rmr_s64` foi fixada
em 64 bits por `stdint.h`, evitando a ambiguidade LP32 de ARMv7.

Artefatos autorais:
- `rmr/hwif/asm/armv7/rmr_hwif_user.S`;
- `rmr/hwif/asm/armv7/rmr_hwif_privileged.S`;
- `rmr/hwif/build/build_cross_matrix.sh`;
- `rmr/hwif/tests/rmr_hwif_selftest.c`;
- `rmr/docs/RMR_ARMV7_HWIF_SPLIT_V1.md`;
- `rmr/benchmark_framework/core/iops_bench.c`;
- `rmr/benchmark_framework/IOPS_PROTOCOL_V1.md`;\n- `rmr/tools/run_device_perf_receipt.sh`.

A matriz cross-arch prova compilação + fechamento de símbolos; link nativo é testado separadamente e link cross-executável permanece TOKEN_VAZIO sem linker no runner. IOPS de CI é smoke do
instrumento e não claim de armazenamento físico. Queue depth >1 permanece
`TOKEN_VAZIO_V1` até implementação assíncrona real.


### Atualização 2026-09-23 (crypto runtime matrix V1)

Criado `rmr/crypto/runtime/` como camada autoral de integração operacional,
sem reimplementar ou renomear BLAKE3. A função BLAKE3 usa a API pública
upstream existente. As demais primitivas usam provider OpenSSL 3 quando
disponível.

Artefatos autorais:
- `rmr/crypto/runtime/include/rmr_crypto_runtime.h`;
- `rmr/crypto/runtime/src/rmr_crypto_common.c`;
- `rmr/crypto/runtime/src/rmr_crypto_blake3.c`;
- `rmr/crypto/runtime/src/rmr_crypto_openssl.c`;
- `rmr/crypto/runtime/tests/rmr_crypto_runtime_selftest.c`;
- `rmr/crypto/runtime/registry.json`;
- `rmr/crypto/runtime/tools/validate_runtime_registry.py`;
- `rmr/crypto/runtime/build/build_contract_matrix.sh`;
- `rmr/crypto/runtime/README.md`;
- `.github/workflows/rmr-crypto-runtime.yml`.

Primitivas: BLAKE3, MD5, SHA-1, SHA-256, SHA-512, HMAC-SHA256,
HKDF-SHA256, Ed25519, ChaCha20-Poly1305 e AES-256-GCM.

MD5 e SHA-1 permanecem `compatibility_only` e não são promovidos para novos
usos de segurança. A aceleração BLAKE3 por SSE/AVX/NEON/WASM não é atribuída
às demais primitivas; algoritmo, provider e arquitetura permanecem domínios
separados. Provider cross-architecture fica `TOKEN_VAZIO_PROVIDER_TOOLCHAIN`
sem toolchain/OpenSSL do alvo.


### Atualização 2026-09-23 (SIMPERF + legal transition docs)

Criado `rmr/benchmark_framework/simperf/` como simulador analítico calibrável
de performance. Os cinco perfis são arquétipos de hardware, não claims sobre
CPUs comerciais específicas. Toda projeção é `ANALOGY_ESTIMATE` e requer
receipt físico para promoção.

Criado `rmr/legal/` para separar estado jurídico atual, transição de licença,
rascunho pesquisa/comercial, protocolo de auditoria/enforcement e checklist
INPI. Esses documentos NÃO alteram `rmr/LICENSE_RMR`; a licença atual continua
permissiva até mudança prospectiva explícita e revisão humana/jurídica.

A segunda expansão crypto adiciona SHA3-256, BLAKE2b-512, HMAC-SHA512, X25519
e PBKDF2-HMAC-SHA256, levando o runtime a 15 algoritmos. A matriz cartesiana
passa a 9 perfis arquiteturais x 15 algoritmos = 135 células.


### Atualização 2026-09-23 (upstream comprehensive validation V3)

A suíte `rmr/upstream_validation/` e o workflow
`.github/workflows/rmr-upstream-comprehensive-v3.yml` separam explicitamente
correção, performance, backend/dispatch, oneTBB, Rust/CLI, ABI/ELF, cross-arch,
sanitizers, instalação/consumer e integração RMR.

Os comparadores usam `BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`
como referência oficial fixada e o exact-head do fork como alvo. Nenhum PASS
de uma superfície é herdado por outra. Execução física ARM e reprodução
independente permanecem TOKEN_VAZIO até receipts observáveis.


### Atualização 2026-09-23 (Full Validation Campaign V4)

Registrados como artefatos autorais RMR:
- `rmr/validation/FULL_VALIDATION_MANIFEST_V4.json`;
- `rmr/docs/RMR_FULL_VALIDATION_CAMPAIGN_V4.md`.

A campanha não altera o algoritmo BLAKE3 upstream. Ela orquestra e documenta
comparações oficiais-vs-fork nos eixos source, C correctness/performance,
ablation, SIMD, TBB, ABI/ELF, Rust, b3sum, cross-arch e integração RMR.
Execução física ARM/IOPS e reprodução independente permanecem TOKEN_VAZIO sem
receipts correspondentes.


### Atualização 2026-09-23 (device-bound upstream comparison V3)

Registrado `rmr/tools/run_physical_upstream_compare_v3.sh` como executor
autoral RMR para comparação física upstream oficial × fork em x86-64, ARMv7 e
AArch64. O executor fixa o upstream por SHA, usa o mesmo harness dos dois lados,
alterna a ordem das rodadas e materializa CSV/JSON/ambiente/SHA-256.

A existência do executor não é evidência de execução física. ARMv7/AArch64
permanecem `TOKEN_VAZIO_PHYSICAL` até receipt produzido no dispositivo.


### Atualização 2026-09-23 (Full Validation Evidence V4)

Registrados:
- `rmr/validation/FULL_VALIDATION_EVIDENCE_V4.json`;
- `rmr/docs/RMR_FULL_VALIDATION_EVIDENCE_V4.md`.

O ledger preserva resultados observados por run/SHA e separa receipts históricos
do exact-master atual. Paridade, regressões, FAIL e TOKEN_VAZIO permanecem
append-only; nenhum resultado histórico é promovido automaticamente após merge.


### Atualização 2026-09-23 (validation completeness V4)

Os agregadores de validação passaram a separar
`REPORT_GENERATION=PASS` de `VALIDATION_STATE`.
Estados globais: COMPLETE, COMPLETE_WITH_REVIEW e PARTIAL.

Adicionado `rmr/validation/tests/test_report_completeness.py` para garantir
que a ausência deliberada de um eixo obrigatório resulte em PARTIAL e nunca
seja mascarada como validação integral.

### μWRITE — upstream 1.8.7 synchronization branch

`μID=MU-BLAKE3-UPSTREAM-1.8.7-20260923T100830Z`  
`ts=2026-09-23T10:08:30Z`  
`source/ref=BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b`  
`parent=141f8065e2c3845978cff7aabb6eeadebc155d30`  
`kind=UPSTREAM_BASELINE_SYNC`  
`Δsummary=39 upstream paths synchronized to 1.8.7; 35 clean imports; semantic preservation in Cargo.toml, README.md, c/CMakeLists.txt and c/blake3.h; sync commit=774ba7ec9639a79132a6992e35f16035707f58d9`  
`routes=P/C/R/I/E/A; L=version-lineage; O=upstream-vs-fork; T=build+runtime`  
`evidence=source/tree/commit materialized; execution gates pending`  
`gap=CI exact-head; common-harness rerun; physical ARMv7/AArch64; independent reproduction`  
`next=open draft PR and require full-validation/comprehensive gates`  
`claim_allowed=false`

