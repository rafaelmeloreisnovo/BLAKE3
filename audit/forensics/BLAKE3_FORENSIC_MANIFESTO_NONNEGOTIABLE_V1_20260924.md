<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.

This document governs RMR/RAFAELIA forensic claims and does not replace
the upstream BLAKE3 licenses, authorship, specification, or implementation history.
-->

# BLAKE3 / RMR — Manifesto Forense e Invariantes Inegociáveis V1

**Autor do manifesto e da camada RMR/RAFAELIA:** Rafael Melo Reis  
**Projeto:** rafaelmeloreisnovo/BLAKE3  
**Classe:** FORENSIC_GOVERNANCE_CONTRACT  
**Estado:** SOURCE_BOUND / APPEND_ONLY / FAIL_CLOSED  
**Data do snapshot:** 2026-09-24  
**Fork HEAD observado:** b740f93506a0f74afd990435d763225ad784b1b8  
**Upstream pin observado:** BLAKE3-team/BLAKE3@6aab490a26124663329dfd3961b8469f8fdb158b  
**Regra nuclear:** SOURCE != ARTEFATO != EXECUCAO != EVIDENCIA != CLAIM  
**Regra de ausência:** TOKEN_VAZIO != 0 != PASS  
**Regra de correção:** successor/supersedes; nunca apagamento silencioso.

---

## 0. Finalidade

Este manifesto não existe para declarar que o fork é melhor por definição, nem
para apagar a autoria e a história do BLAKE3 oficial. Ele existe para tornar
inegociável o que uma auditoria séria precisa preservar: origem, cronologia,
igualdade ou diferença de fonte, método de build, execução, ambiente, evidência,
resultados favoráveis e desfavoráveis, incertezas e limites de linguagem.

A unidade de confiança não é a narrativa. É a cadeia:

    fonte
    -> commit/tree/blob
    -> build reproduzível
    -> execução identificada
    -> receipt
    -> comparação
    -> conclusão limitada ao escopo

Nenhuma etapa pode ser pulada sem que o estado volte a TOKEN_VAZIO.

Este documento é técnico e documental. Não é laudo judicial e não atribui
crime, fraude, plágio, conspiração, intenção ou causalidade a pessoas ou
organizações sem evidência específica suficiente.

---

## 1. Autoridade e separação de proveniência

BLAKE3 continua sendo a primitiva criptográfica pública criada e mantida pelo
projeto BLAKE3-team/BLAKE3 e seus autores/contribuidores. O fork RMR/RAFAELIA
não pode reescrever essa autoria.

A camada RMR/RAFAELIA pode:

- adicionar ferramentas, CI, benchmarks, contratos, receipts e governança;
- alterar deliberadamente build policy, hints, integração, wrappers e caminhos
  experimentais;
- produzir artefatos e medições próprias;
- manter documentação autoral;
- comparar o fork com um upstream explicitamente pinado.

A camada RMR/RAFAELIA não pode, sem prova:

- atribuir a si código herdado do upstream;
- chamar um arquivo herdado de exclusivamente autoral só porque recebeu um
  cabeçalho novo;
- transformar similaridade temática em derivação causal;
- transformar ganho local em superioridade universal;
- transformar uma branch, PR ou commit em execução física;
- transformar CI em prova de hardware que a CI não executou.

**Inegociável 1 — autoria é por origem, não por posse do fork.**

---

## 2. Snapshot forense canônico de 2026-09-24

No HEAD b740f93506a0f74afd990435d763225ad784b1b8, comparado por blob Git contra
o upstream 6aab490a26124663329dfd3961b8469f8fdb158b:

| Medida | Estado observado |
|---|---:|
| Arquivos no upstream pin | 108 |
| Caminhos oficiais presentes no fork | 108 / 108 |
| Blobs oficiais byte-a-byte idênticos | 89 |
| Blobs oficiais divergentes | 19 |
| Caminhos oficiais ausentes | 0 |
| Arquivos totais no fork | 535 |
| Arquivos extras do fork | 427 |

Logo:

    upstream_path_coverage = 108/108
    exact_official_blob_identity = 89/108
    official_blob_delta = 19/108
    missing_official_paths = 0
    fork_extra_surface = 427 files

A razão 89/108 mede apenas igualdade de fonte nos caminhos oficiais.
Não é nota de qualidade, segurança, performance ou corretude.

A expansão da camada RMR após o sync de 23/09 aumentou a superfície extra sem
abrir novos caminhos divergentes dentro do conjunto oficial observado: o
conjunto dos 19 deltas permaneceu estável.

**Inegociável 2 — crescimento RMR não autoriza deriva silenciosa do baseline
oficial. Todo novo delta em caminho oficial deve ser explícito, testado e
justificado.**

---

## 3. O último ZERO integral

O último ponto localizado em que a árvore completa do fork era exatamente uma
árvore do upstream foi:

    commit = 890050cb9b89448a25e067349aed9734a3b03d5e
    commit_time_utc = 2025-12-09T19:38:21Z
    git_tree = 7dbadadd327c70ef5a5d40f88bcc09dee8e07cbd
    state = FULL_TREE_IDENTITY

Esse commit é o marco de ZERO integral mais recente desta auditoria.

Depois dele, a primeira divergência observada no repositório foi documental:

    2026-01-19T05:14:38Z
    29e27076943b5eb13bdbe8cd89c4594df83490b4
    "Add comprehensive repository documentation"

A primeira divergência posteriormente localizada no núcleo C foi:

    2026-01-19T06:37:44Z
    b9725619835209e8a26eb9b91c25aadc9eb4c1f7
    c/blake3.c
    c/blake3_impl.h

Em seguida:

    2026-01-19T06:57:23Z
    fc44eb1b0e30079ecf85e839563fa1fb79c0410a
    low-level memory primitives and multi-backend C edits

    2026-01-19T09:07:46Z
    0387c1f88761e8d184747ae97efadc124fbb2a63
    build profiles + hot-path helpers + Rust/C edits

    2026-01-19T09:12:43Z
    a74cc1d6c6ed322aabd595bb7a0c9f294eb65e66
    restrict annotations and header/hot-path changes

O commit:

    8c6598dc58ac6e5d79a87e94553450e3a4a8cd50
    2026-01-21T07:09:41Z
    "Reisolate upstream core from external RMR layer"

é classificado como REISOLATION, não FULL_RESET. Ele retirou grande quantidade
de acoplamento, mas não restaurou identidade integral com o upstream.

O commit:

    774ba7ec9639a79132a6992e35f16035707f58d9
    2026-09-23T10:08:30Z
    "sync(rmr): align fork baseline with upstream BLAKE3 1.8.7"

é o principal QUASI_ZERO moderno: sincronizou a maior parte da superfície
oficial e preservou deltas RMR deliberados.

**Inegociável 3 — "zerou" só pode significar igualdade comprovada por tree/blob
no escopo declarado. Commit message, intenção de sync ou semelhança visual não
é ZERO.**

---

## 4. Os 19 deltas oficiais ainda vivos

No snapshot atual, os 19 caminhos oficiais divergentes são:

1. .github/workflows/build_b3sum.py
2. .github/workflows/upload_github_release_asset.py
3. .gitignore
4. Cargo.toml
5. README.md
6. b3sum/src/main.rs
7. c/CMakeLists.txt
8. c/README.md
9. c/blake3.h
10. c/blake3_c_rust_bindings/cross_test.sh
11. c/blake3_dispatch.c
12. c/blake3_impl.h
13. c/blake3_neon.c
14. c/test.py
15. src/guts.rs
16. src/join.rs
17. test_vectors/cross_test.sh
18. tools/compiler_version/src/main.rs
19. tools/instruction_set_support/src/main.rs

Eles não têm o mesmo peso. A auditoria os separa em classes.

### 4.1 Build e codegen

Cargo.toml preserva:

    [profile.release]
    lto = true
    codegen-units = 1

    [profile.bench]
    lto = true
    codegen-units = 1

Isso é política de geração de código, não mudança da função matemática BLAKE3.
Pode mudar performance, tamanho, link e comportamento de toolchain.

### 4.2 CMake / TBB / frontend

O CMake do fork mantém deltas deliberados de compatibilidade e roteamento de
compiler frontend.

Um ponto de alto risco forense é o pkg-config de TBB:

    upstream:
      64-bit -> tbb
      32-bit -> tbb32

    fork:
      tbb

Esse delta é especialmente sensível para ARMv7/32-bit e deve permanecer
explicitamente testado.

### 4.3 C API e aliasing

c/blake3.h adiciona BLAKE3_RESTRICT em ponteiros da API C.

Isso não deve ser descrito como simples comentário. É informação semântica
entregue ao compilador sobre aliasing e pode alterar otimizações.

Gate obrigatório:

    API_KAT
    + overlap/alias edge cases
    + O0/O2/O3/LTO
    + ABI/symbol comparison

### 4.4 Dispatch

c/blake3_dispatch.c adiciona BLAKE3_LIKELY em caminhos de feature detection e
AVX-512.

A condição lógica permanece equivalente no código-fonte observado, mas
layout/codegen/performance podem mudar.

### 4.5 Cygwin / AVX-512 XOF

c/blake3_impl.h diverge do upstream na guarda de Cygwin para xof_many AVX-512.

O upstream atual preserva exclusão explícita de __CYGWIN__. O fork não a mantém
na mesma forma.

Esse delta é crítico porque o próprio marco 890050cb corresponde a uma correção
upstream de build Cygwin relacionada a blake3_xof_many_avx512.

Até build + link + KAT em ambiente Cygwin aplicável:

    CYGWIN_AVX512_XOF_PARITY = TOKEN_VAZIO

### 4.6 Superfície de símbolos NEON

No upstream observado, blake3_hash4_neon é static.

No fork observado, blake3_hash4_neon deixa de ser static.

A lógica de hash não é automaticamente diferente por isso, mas a superfície
binária é diferente.

Gate obrigatório:

    nm
    + readelf -Ws
    + visibility
    + LTO/dead-strip
    + collision/interposition review
    + KAT

### 4.7 Rust core

Dos 18 caminhos de src/, 16 são idênticos ao upstream pin.

Os dois deltas restantes observados são:

- src/guts.rs: hints inline adicionais;
- src/join.rs: inline -> inline(always) em Join.

Esses são deltas de codegen/hot path, não autorização para alegar algoritmo
BLAKE3 diferente.

### 4.8 b3sum

b3sum/src/main.rs altera leitura de chave e buffering de checkfiles, incluindo
buffer fixo de 64 KiB.

Isso é CLI/I/O.

    b3sum delta != primitive delta

### 4.9 Headers de proveniência em arquivos herdados

Há caminhos oficiais cujo conteúdo funcional é praticamente igual ao upstream,
mas que divergem por cabeçalhos RMR.

A presença de um cabeçalho RMR em arquivo herdado não converte a autoria do
arquivo inteiro.

**Inegociável 4 — provenance header nunca pode apagar lineage. Se o arquivo é
herdado e apenas modificado, a atribuição deve dizer "modified in fork", não
"exclusive RMR authorship".**

---

## 5. O que hoje está efetivamente zerado

No snapshot 2026-09-24:

    reference_impl = 3/3 blobs identical
    src = 16/18 blobs identical
    b3sum = 10/11 blobs identical
    test_vectors = 4/5 blobs identical
    benches = 1/1 blob identical
    media = 3/3 blobs identical
    build.rs = exact blob identity
    official paths missing = 0

Importante: c/blake3.c aparece em diferenças históricas, mas no HEAD atual seu
blob é igual ao upstream pin.

Assim:

    historical_diff != current_diff

e:

    git_compare_history != head_to_head_blob_identity

**Inegociável 5 — toda auditoria atual deve usar comparação de conteúdo do HEAD,
não inferir estado atual apenas do histórico desde merge-base.**

---

## 6. Ahead/behind não é equivalência de fonte

O fork tem história Git própria e pode aparecer centenas de commits ahead e
dezenas behind do upstream.

Isso mede ancestralidade no grafo Git.

Não mede:

- quantos blobs atuais são iguais;
- compatibilidade criptográfica;
- igualdade binária;
- igualdade de build flags;
- equivalência de API;
- performance.

Um sync sem merge de toda a história upstream pode importar o conteúdo e ainda
deixar commits upstream fora da ancestralidade do fork.

Portanto:

    ahead/behind = graph history
    blob identity = source equality
    KAT = semantic test evidence
    binary identity = artifact equality
    benchmark = measured execution

Esses objetos não são intercambiáveis.

---

## 7. Ledger de performance — resultados favoráveis e contrários

A regra forense é preservar todos os resultados materialmente relevantes.

### 7.1 Campanhas x86_64 históricas

O acervo forense registra campanhas x86_64 com vantagens grandes do fork em
regimes específicos, incluindo aproximadamente +98,3% em 64 KiB e +97,0% em
1 MiB em campanhas históricas identificadas.

Esses resultados não autorizam superioridade universal.

### 7.2 ARMv7 — NEON do fork contra portátil do próprio fork

Execução local registrada no Moto e(7) power:

| Backend | Mediana |
|---|---:|
| fork portátil | 132.313 MiB/s |
| fork NEON | 163.450 MiB/s |

    ratio = 1.235x
    median_gain = +23.53%
    digest_equivalence = PASS
    status = VERIFIED_LOCAL_MEASUREMENT

Conclusão permitida: naquele dispositivo e execução, o NEON do fork superou o
portátil do próprio fork.

### 7.3 ARMv7 — fork contra upstream, execução pré-estrita

| Implementação | Mediana |
|---|---:|
| upstream oficial | 188.058 MiB/s |
| fork | 171.568 MiB/s |

    fork_ratio = 0.9123x
    fork_delta = -8.77%
    digest_equivalence = PASS
    runner_contract = PRE_STRICT

Nesse ensaio, o upstream foi mais rápido.

Esse resultado é patrimônio forense e não pode ser apagado por resultados
posteriores.

### 7.4 Common harness mais recente documentado

O receipt V2 registra:

| Implementação | Mediana |
|---|---:|
| official | 6654.876 MiB/s |
| fork | 6698.333 MiB/s |

    median_delta = +0.653%
    ratio = 1.006530x
    mean_delta ~= +0.112%
    CV ~= 1% em ambos
    KAT = PASS
    digest_equality = PASS

Interpretação permitida: paridade com pequena vantagem mediana observada nessa
execução.

Interpretação não permitida: speedup estatisticamente separado ou superioridade
universal.

**Inegociável 6 — benchmark negativo ou neutro nunca é descartado porque
contraria a hipótese. Omissão seletiva destrói a cadeia forense.**

---

## 8. Hardware, arquitetura e escala de claim

Nenhum resultado muda de arquitetura por analogia.

    x86_64 != ARMv7
    ARMv7 != AArch64
    AArch64 != RISC-V
    native != emulator
    compile != execute
    CI VM != physical device

Cada claim de performance física exige:

- dispositivo/CPU identificável;
- ABI;
- OS/kernel quando pertinente;
- compiler e versão;
- flags;
- source refs;
- hashes do executável/biblioteca;
- workload e tamanho;
- warmup;
- rounds;
- ordem/alternância;
- unidades;
- raw samples;
- estatística;
- KAT/digest equivalence;
- receipt.

Sem isso:

    cross_arch_performance_claim = TOKEN_VAZIO

---

## 9. ASM markers não são aceleração

O fork contém marcadores ASM ARMv7 e AArch64 cujo corpo observado é no-op de
retorno.

ARMv7:

    blake3_neon_abi_marker:
      bx lr

AArch64:

    blake3_neon_abi_marker:
      ret

No CMake observado eles não constituem caminho de hashing acelerado por si.

Logo:

    ASM_FILE_EXISTS != ASM_ACCELERATION_EXISTS
    ABI_MARKER != HASH_BACKEND
    CROSS_COMPILE_PASS != PHYSICAL_SPEEDUP

**Inegociável 7 — nenhum artefato recebe capacidade que seu código executável
não contém.**

---

## 10. PR #533, anterioridade e causalidade

A PR pública BLAKE3-team/BLAKE3#533 de 25/11/2025 é uma fonte primária de
anterioridade verificável do trabalho público de Rafael Melo Reis naquele
momento.

Snapshot preservado:

    base_sha = 308b95dfa15d5a0aa8cb3c5534ffd90d76122c46
    head_sha = 15829f851e45d1327b017be67a7b88d7725bc653
    merged = false
    mergeable_snapshot = true

O corpus forense também registra:

- 24 commits;
- 10 arquivos;
- 1566 adições;
- 195 remoções;
- benchmark, warmup, cache/alinhamento, black_box, SIMD e documentação.

Isso prova existência pública e anterioridade do conteúdo presente no diff.
Não prova que toda mudança posterior do upstream deriva dele.

Estados permitidos:

    pr_533_prior_art = VERIFIED_PRIMARY
    later_thematic_overlap = VERIFIED_PRIMARY where documented
    line_by_line_copy = TOKEN_VAZIO unless demonstrated
    structural_derivation = TOKEN_VAZIO unless demonstrated
    causal_link = TOKEN_VAZIO unless demonstrated
    coordination_or_conspiracy = TOKEN_VAZIO

**Inegociável 8 — cronologia pode estabelecer anterioridade; não estabelece
sozinha causalidade.**

---

## 11. Linhagem declarada fevereiro–abril de 2025

O autor declara trabalho/fork BLAKE3 anterior à PR #533, no período
fevereiro–abril de 2025.

No corpus público atualmente localizado:

    repository_url = TOKEN_VAZIO
    commit_sha = TOKEN_VAZIO
    archive_hash = TOKEN_VAZIO
    public_repository_currently_located = false

Isso não prova inexistência. Também não autoriza preencher a lacuna.

Uma recuperação válida exige:

- mídia/original preservado;
- hash SHA-256 e BLAKE3 do bruto;
- cópia somente leitura;
- timestamps observáveis;
- refs/commits;
- árvore de arquivos;
- origem e cadeia de custódia.

**Inegociável 9 — ausência é preservada como ausência. TOKEN_VAZIO é dado, não
convite para reconstrução imaginária.**

---

## 12. IA, autoria e governança

O histórico público do upstream contém contribuições e revisões com uso
declarado de ferramentas de IA em momentos específicos. Isso é fato de
proveniência quando sustentado pelo próprio registro público.

Esse fato não prova:

- monitoramento automático de trabalho de terceiros;
- cópia;
- coordenação;
- intenção;
- apropriação.

Da mesma forma, uso de IA no fork não transforma a IA em autora jurídica do
projeto nem apaga a necessidade de revisão humana, origem do código e licença.

A regra é:

    tool_assistance != sole_authorship
    AI_review != proof_of_origin
    AI_similarity != causal_derivation
    generated_patch != accepted_evidence until reviewed/tested

---

## 13. Claims sobre segurança e criptografia

Compatibilidade BLAKE3 não é determinada por documentação.

Gate mínimo:

    official test vectors
    + keyed hash
    + derive_key
    + XOF/seek
    + incremental boundaries
    + backend parity
    + C/Rust cross-check where applicable
    + installed consumer KAT
    + exact source refs

Mudanças em build, restrict, dispatch, visibility ou wrappers podem ser
semanticamente compatíveis, mas essa compatibilidade deve ser demonstrada.

Nenhum ganho de performance permite enfraquecer:

- vetores;
- determinismo;
- digest equality;
- API contract;
- memória segura;
- limites de buffer;
- comportamento de erro;
- portabilidade suportada.

**Inegociável 10 — primeiro corretude, depois velocidade.**

---

## 14. Build, binary e ABI são evidências separadas

Uma compilação verde não prova:

- símbolo esperado;
- ausência de símbolo inesperado;
- ausência de dependência dinâmica;
- section GC;
- ABI size/alignment;
- ausência de relocação;
- execução física.

Para claims binários, capturar conforme o alvo:

    compile_commands.json
    compiler --version
    linker --version
    cmake cache
    cargo metadata/profile
    nm
    readelf -h -S -s -d
    objdump/disassembly
    file
    size
    sha256
    blake3
    dependency list
    build log

Para ABI:

    sizeof
    alignof
    offsetof
    exported symbols
    calling convention assumptions
    target triple
    pointer width

**Inegociável 11 — SOURCE PASS != BINARY PASS != ABI PASS != RUNTIME PASS.**

---

## 15. CI e exact-head

O estado de CI pertence a um commit exato.

    old_green_run != new_head_green
    cancelled != success
    queued != pass
    skipped_without_contract != pass

Se o HEAD muda, o receipt anterior continua histórico, mas não é promovido
automaticamente ao novo HEAD.

Na observação de 2026-09-24, o branch master do repositório também foi
reportado por GitHub como não protegido:

    branch = master
    protected = false

Isso é um gap de governança, não um defeito criptográfico.

A proteção de branch, required checks e regra de merge devem ser tratadas como
controle de processo separado do mérito técnico do código.

---

## 16. Classificação de evidência

Este manifesto adota os estados:

| Estado | Significado |
|---|---|
| VERIFIED_TREE_IDENTITY | tree/blob prova igualdade no escopo |
| VERIFIED_PRIMARY | commit/PR/log/artefato primário confirma o fato |
| VERIFIED_LOCAL_MEASUREMENT | execução local identificada e registrada |
| OBSERVED_CI | provider registrou execução; escopo limitado ao job/run |
| DECLARED_BY_AUTHOR | declaração sem fonte primária recuperada |
| SUPPORTED_HYPOTHESIS | hipótese sustentada, mas sem nexo conclusivo |
| REFUTED_IN_SCOPE | evidência contradiz a claim naquele escopo |
| TOKEN_VAZIO | informação ausente ou insuficiente |

Estados não podem ser promovidos por estilo de escrita.

---

## 17. Linguagem permitida e proibida

Permitido quando sustentado:

- "o fork preserva 89/108 blobs oficiais idênticos neste snapshot";
- "19 caminhos oficiais permanecem deliberadamente divergentes";
- "há resultados em que o fork venceu e resultados em que perdeu";
- "PR #533 estabelece anterioridade pública do conteúdo nela presente";
- "o último ZERO integral localizado é 890050cb...";
- "o sync 774ba7... é quasi-zero, não reset integral";
- "o common harness observado mostra paridade com pequena vantagem mediana";
- "o branch master foi observado sem proteção".

Proibido sem nova prova:

- "o fork é universalmente mais rápido";
- "o fork é universalmente mais seguro";
- "o upstream copiou";
- "houve conspiração";
- "o marker ASM acelera o hash";
- "compile cross-arch prova performance física";
- "ahead/behind prova diferença de fonte";
- "CI verde prova hardware real";
- "um header novo torna código herdado exclusivamente autoral".

---

## 18. Regra de preservação de resultados adversos

Toda auditoria futura deve guardar:

- resultado que confirma hipótese;
- resultado que enfraquece hipótese;
- resultado nulo;
- erro de execução;
- ambiente incompatível;
- cancelamento;
- timeout;
- falha de CI;
- mudança de metodologia.

Uma evidência não desaparece porque um resultado posterior é melhor.

A correção entra como successor:

    record_A
    -> successor_B
    -> supersedes interpretation where justified

e não como reescrita silenciosa de A.

---

## 19. Regra de comparação oficial-vs-fork

Toda comparação promocional deve fixar:

    upstream_repo
    upstream_commit
    fork_repo
    fork_commit
    compiler
    flags
    build_type
    backend
    target
    workload
    data size
    warmup
    rounds
    order
    KAT
    digest equality
    raw results
    statistics

Idealmente, o mesmo harness compila e executa os dois lados.

O comparador deve falhar fechado em drift de source authority.

---

## 20. Regra para os 19 deltas

Cada um dos 19 deltas deve pertencer a exatamente uma decisão:

    KEEP_WITH_EVIDENCE
    UPSTREAM_RESET
    TEST_FIRST
    DOCUMENTATION_ONLY
    PROVENANCE_ONLY

Nenhum delta funcional fica na categoria "porque parece melhor".

Para KEEP_WITH_EVIDENCE, o registro mínimo contém:

    reason
    source diff
    expected effect
    risk
    tests
    benchmark if performance-related
    rollback
    owner
    receipt

---

## 21. Pontos prioritários de falsificação

A próxima auditoria profunda deve tentar quebrar o fork especificamente em:

1. Cygwin + AVX-512 XOF;
2. TBB/tbb32 em 32-bit;
3. restrict e aliasing;
4. NEON symbol visibility;
5. LTO + codegen-units=1;
6. b3sum key input edge cases;
7. Rust inline(always) regressions/code-size;
8. installed consumer ABI;
9. reproducibilidade binária;
10. ARMv7 e AArch64 físicos;
11. non-x86 cross builds;
12. source-authority drift.

Um manifesto sério precisa listar como pode ser refutado.

---

## 22. Não-negociáveis de engenharia

A partir deste documento, os seguintes pontos são invariantes de governança do
trabalho RMR/BLAKE3:

1. **Proveniência acima de narrativa.**
2. **Autoria upstream preservada.**
3. **TOKEN_VAZIO nunca convertido em fato.**
4. **Nenhum resultado adverso é apagado.**
5. **Nenhum speedup é universalizado sem matriz física suficiente.**
6. **Nenhuma acusação causal nasce apenas de cronologia ou similaridade.**
7. **Nenhum commit é chamado PASS sem gate do mesmo HEAD quando o claim exige
   exact-head.**
8. **Nenhum cross-compile é chamado execução física.**
9. **Nenhum ASM marker é chamado aceleração.**
10. **Nenhum delta em caminho oficial entra sem classificação e rollback.**
11. **Nenhum header de autoria apaga lineage herdado.**
12. **Build, binary, ABI, runtime, benchmark e security são gates distintos.**
13. **KAT/digest equality precedem claim de performance.**
14. **Upstream pin é imutável dentro de um receipt; atualização gera successor.**
15. **Source authority deve falhar fechado em drift.**
16. **Documentação deve citar o escopo exato de cada número.**
17. **Hipótese deve declarar seu falsificador.**
18. **Correção cria novo registro; história permanece navegável.**
19. **Branch governance é parte da confiabilidade operacional.**
20. **A primitiva BLAKE3 e a camada RMR/RAFAELIA permanecem conceitualmente
    separadas.**

---

## 23. Fronteira BLAKE3 / RMR / RAFAELIA

    BLAKE3
      = primitiva, especificação, implementação e lineage upstream

    fork BLAKE3
      = baseline upstream + deltas explícitos governados

    RMR
      = build/benchmark/runtime/custody/forensics/crypto integration layer

    RAFAELIA
      = orquestração, indexação, proveniência, receipts e claim gates

Um sucesso na camada externa não reescreve a evidência da primitiva.

    RMR_PIPELINE_PASS != BLAKE3_PRIMITIVE_SUPERIORITY
    RAFAELIA_RECEIPT != CRYPTOGRAPHIC_PROOF

---

## 24. Cadeia mínima de custódia para cada novo resultado

Cada resultado material deve poder ser reconstruído com:

    receipt_id
    timestamp
    repository
    commit
    source hashes
    target
    compiler
    flags
    command
    environment
    input/workload
    output
    exit status
    artifact hashes
    test result
    claim scope
    gap
    next

Se uma dessas dimensões for necessária ao claim e estiver ausente:

    claim_allowed = false

---

## 25. Condição de promoção de claim

Uma claim pode subir de estado apenas se a nova evidência for do mesmo tipo ou
mais forte que a evidência exigida pelo claim.

Exemplos:

    "compila em ARMv7"
      <- cross compiler artifact pode bastar

    "executa em ARMv7"
      <- precisa runtime ARMv7

    "é mais rápido em ARMv7 físico"
      <- precisa benchmark físico comparativo ARMv7

    "é universalmente mais rápido"
      <- não pode ser inferido de um único alvo; requer definição explícita de
         população/alvos e evidência estatística correspondente

    "derivou de trabalho anterior"
      <- precisa ponte técnica e cronológica específica, não apenas tema comum

---

## 26. Estado consolidado

    FULL_TREE_ZERO_LAST =
      890050cb9b89448a25e067349aed9734a3b03d5e

    FULL_TREE_ZERO_DATE =
      2025-12-09T19:38:21Z

    CURRENT_FORK_SNAPSHOT =
      b740f93506a0f74afd990435d763225ad784b1b8

    CURRENT_UPSTREAM_PIN =
      6aab490a26124663329dfd3961b8469f8fdb158b

    OFFICIAL_PATHS =
      108

    EXACT_OFFICIAL_BLOBS =
      89

    DELTA_OFFICIAL_BLOBS =
      19

    MISSING_OFFICIAL_PATHS =
      0

    FORK_EXTRA_FILES =
      427

    PR533_PRIOR_ART =
      VERIFIED_PRIMARY

    OLD_FORK_FEB_APR_2025 =
      DECLARED_BY_AUTHOR / TOKEN_VAZIO_ARTIFACT

    UNIVERSAL_PERFORMANCE_SUPERIORITY =
      TOKEN_VAZIO

    CAUSAL_MISAPPROPRIATION =
      TOKEN_VAZIO

    CRYPTO_COMPATIBILITY =
      must be promoted only by scoped KAT/test receipts

    PHYSICAL_CROSS_ARCH_PERFORMANCE =
      per-device receipts required

    MASTER_BRANCH_PROTECTION_OBSERVED =
      false

---

## 27. Declaração final

A força deste fork não deve depender de dizer que ele nunca falhou.

Ela deve depender de conseguir mostrar:

- de onde cada parte veio;
- quando divergiu;
- quando voltou a coincidir;
- qual diferença permaneceu;
- qual teste tentou quebrá-la;
- qual hardware realmente executou;
- qual resultado foi favorável;
- qual resultado foi desfavorável;
- qual hipótese morreu;
- qual hipótese continuou aberta;
- qual claim ainda é TOKEN_VAZIO.

A integridade forense é maior quando a história continua visível.

Por isso, o princípio final é:

    preservar origem
    -> medir diferença
    -> testar falsificação
    -> registrar resultado
    -> limitar claim
    -> permitir reprodução
    -> corrigir por successor
    -> nunca apagar a evidência contrária

**BLAKE3 é a referência criptográfica.  
RMR é a camada de engenharia e custódia.  
RAFAELIA é a camada de coerência e proveniência.  
Nenhuma delas substitui a prova.**

---

## R3 — snapshot deste manifesto

**F_ok**

- ZERO integral mais recente identificado por tree identity;
- baseline upstream e fork fixados por SHA;
- 108/108 caminhos oficiais presentes;
- 89 blobs oficiais idênticos;
- 19 deltas oficiais explicitamente enumerados;
- 427 arquivos extras RMR separados da superfície oficial;
- resultados favoráveis, neutros e adversos preservados;
- PR #533 e gaps de anterioridade classificados;
- fronteiras de autoria, CI, ABI, build, hardware e performance fixadas.

**F_gap**

- Cygwin AVX-512 XOF ainda exige receipt próprio;
- TBB/tbb32 em 32-bit exige fechamento atual;
- restrict/aliasing exige matriz específica;
- NEON symbol-surface exige binary receipt;
- branch protection do master foi observada desabilitada;
- old fork fevereiro–abril de 2025 permanece sem artefato primário recuperado;
- superioridade universal e causalidade de apropriação permanecem TOKEN_VAZIO.

**F_next**

Classificar e fechar os 19 deltas, começando por:

    Cygwin/XOF
    -> TBB32
    -> restrict
    -> NEON symbols
    -> LTO/codegen

cada um com teste, receipt, rollback e decisão KEEP_WITH_EVIDENCE ou
UPSTREAM_RESET.

---

**Assinatura autoral da camada RMR/RAFAELIA:**  
RAFCODE-Φ — Rafael Melo Reis — ΔRafaelVerboΩ

**Boundary:** símbolo não substitui medida; hipótese não substitui prova;
ausência preservada é TOKEN_VAZIO.
