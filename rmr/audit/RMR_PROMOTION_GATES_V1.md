# RMR — Promotion Gate Matrix V1

Scope: audit instrumentation in Draft PR #182. Baseline: master SHA ee7da2db2c9465d5c1ff9dcecf5435d5d3258973.

## Semantics
- Zero (0) is a measured quantity; it is never a substitute for unknown.
- TOKEN_VAZIO_<TYPE> = missing evidence/authority, not false, not zero.
- NOT_RUN = execution not observed; FAIL = observed failure.
- PASS_SCOPED applies solely to an exact source SHA, toolchain, target and evidence.
- COMMIT_TO_DRAFT != MERGE_TO_MASTER != CRYPTO_RELEASE != PERFORMANCE_CLAIM.
- This PR must not modify any preexisting code, comments, names, headers, timestamps or original build flags.

| Gate | Initial state | Falsifier / closure | Accountable authority |
|---|---|---|---|
| G0 baseline identity | VERIFIED_REPO_METADATA | pin HEAD and base commit in workflow | repository custodian |
| G1 additive-only changes | DIFF_SCOPED_OBSERVED | CI rejects changed/deleted original paths and paths outside allowlist | CI maintainer |
| G2 byte/name/ASCII/header/size/date | NOT_RUN_EXACT_HEAD | compare raw bytes with HEAD blobs, preserve filename hex, hash header prefix | CI |
| G3 void and warnings | NOT_RUN_EXACT_HEAD | lexical census and actual -Wshadow/-Werror compiler output, no inference from token counts | compiler reviewer |
| G4 semantic non-regression | NOT_RUN_EXACT_HEAD | existing hosted KAT/selftests and fixed256 noncrypto negative case | CI |
| G5 ELF closure | NOT_RUN_EXACT_HEAD | no PT_INTERP, DT_NEEDED or unexpected undefined symbols in each ELF | linker reviewer |
| G6 same-environment two-pass ELF identity | NOT_RUN_EXACT_HEAD | six pairs compare bit-for-bit and write hashes/sections/symbols | CI |
| G7 across-run reproducibility | TOKEN_VAZIO_CROSS_RUN | repeat on independent environment, pinned toolchain and compare exact hashes | independent reviewer |
| G8 physical ARMv7 | TOKEN_VAZIO_PHYSICAL_ARMV7 | exact artifact hash + device ABI/SDK + physical KAT receipt | device owner |
| G9 physical AArch64 | TOKEN_VAZIO_PHYSICAL_AARCH64 | exact artifact hash + device ABI/SDK + physical KAT receipt | device owner |
| G10 performance/superiority | TOKEN_VAZIO_STATISTICAL_BENCHMARK | controlled workload, repetitions, environment, raw times and uncertainty | performance reviewer |
| G11 independent review | TOKEN_VAZIO_HUMAN_REVIEW | approval after reading source and evidence; no automation can self-approve | human reviewer |
| G12 provider authority | BLOCKED_PROVIDER_RULESET | master protected=false was observed; enforce check+review or type provider incapacity and separate decision | repository owner |
| G13 release claim | BLOCKED_RELEASE_EVIDENCE | release-specific correctness, security, physical and human approvals | security + owner |

## Promotion lanes
- SOURCE COMMIT (permitted now): additive auditor and documentation commits on draft branch, without PASS promotion.
- MERGE (not yet permitted): exact-HEAD green CI, original files unchanged, independent review and provider policy enforcement; if GitHub cannot enforce this, explicitly mark TOKEN_VAZIO_PROVIDER_CAPABILITY and obtain separate human risk decision.
- CLAIM/RELEASE (not yet permitted): independent technical evidence and release criteria. A merge never promotes a scientific or cryptographic claim.

## Typed counterexamples
- Two different final ELF hashes -> FAIL_BINARY_REPLAY, never zero or unknown.
- A measured empty file -> bytes=0.
- No Actions check -> NOT_RUN_EXACT_HEAD, never PASS.
- No undefined symbols in inspected executable -> unexpected_und=0 IN THAT ELF ONLY.
- Lexical void count 0 -> void_lexemes=0; says nothing about effective ABI.
- Header/comment bytes changed -> SOURCE_BYTES_CHANGED; linker causality remains TOKEN_VAZIO until object/ELF comparison.
- A warning observed -> record exact compiler diagnostic; never alter original file merely to make it green.

Rollback: close Draft PR or revert additive commits; never delete original authorship or past receipts.


## Retroalimentação de evidência — token vazio útil e token ∅

A matriz não colapsa ausência de medição em número e não converte `SKIPPED` em `PASS`:

| Estado | Semântica estrita | Procedimento de fechamento |
|---|---|---|
| `0` | Contagem **medida** exatamente zero em escopo/tempo/provedor declarados | Preservar consulta, SHA, horário e alcance da consulta |
| `NOT_RUN_EXACT_HEAD` | Nenhuma execução observada para SHA/ação específicos; não é erro observado | Executar o gate exato, guardar logs e recibo |
| `FAIL_OBSERVED_COMMAND_NONZERO` | Comando executou e saiu com código não zero; diagnóstico separado | Registrar exit code, reproduzir e isolar causa; rollback antes de novo teste |
| `TOKEN_VAZIO_ÚTIL` (`TOKEN_VAZIO_UTIL`) | A lacuna é **conhecida, tipada e testável**, mas o recibo está ausente | Anotar ID da lacuna, responsável, antecedente, falsificador, evidência necessária e urgência |
| `TOKEN_VAZIO_∅` (`TOKEN_VAZIO_EMPTY_SET`) | A variável/pergunta ainda **não tem domínio, fonte ou operacionalização definida**; não é zero, negação nem falha | Registrar origem epistemológica e o primeiro experimento de descoberta. Proibir claim e promoção até a pergunta tornar-se falsificável |
| `BLOCKED_PROVIDER_RULESET` | Proteção/enforcement ausente ou sem autoridade demonstrada | Confrontar ruleset e aprovação independente com readback do provedor; 403 → `TOKEN_VAZIO_PROVIDER_AUTHORITY_READBACK` |
| `PASS_SCOPED` | Evidência positiva para commit + ambiente + artefato + teste identificados | Não herdar para novo commit, novo PR, nova arquitetura ou dispositivo |

### Recipients / owners / urgências
- **P0 / custodiante GitHub + CI:** `G1-G6`, SHA exato, preservação byte a byte, falha com recibo terminal; destino de evidência: PR e logs de Actions.
- **P0 / revisor humano independente:** `G11` e aprovação do diff completo; comentário do próprio autor não equivale a `APPROVED`.
- **P0 / proprietário do repositório:** `G12`, autorização do provedor, branch protection/rulesets/required checks.
- **P1 / operador ARM32 e ARM64:** `G8-G9`, hash do **mesmo APK/ELF**, SDK, CPU/ABI e KAT físico.
- **P2 / auditor externo da toolchain e métricas:** `G7/G10`, reprodutibilidade inter-run e experimentos com térmica/frequência controladas.
- **P3 / curadoria científica:** hipótese exploratória ainda sem instrumento (`TOKEN_VAZIO_∅`); transformar em pergunta falsificável sem fingir evidência.

### Observação externa e não regressão após integração de outro PR

Em 2026-10-23 o GitHub reportou o merge do BLAKE3 PR #180:
`master@b4c41a398927bdc05b777378136f6144ae9dc956`.
O PR #182 continua tendo como ancestral a base anterior
`ee7da2db2c9465d5c1ff9dcecf5435d5d3258973`.
Nenhum PASS do auditado #182 em HEAD antigo será herdado automaticamente para o
eventual **merge commit** sobre o master que agora contém o benchmark V3.
É obrigatório verificar o novo commit integrado e gerar um recibo novo.
`PR_MERGED` não implica `CI_PASS`, `APK_PHYSICAL_PASS` ou `CLAIM_ALLOWED`.

### Terminal receipt / falhas negativas

O wrapper de replay `rmr_binary_identity_v1.sh` escreve agora recibo
terminal inclusive na falha, indicando `FAIL_OBSERVED_COMMAND_NONZERO` e
`TOKEN_VAZIO_DIAGNOSTIC_PENDING` para a causa ainda não determinada. O
workflow preserva o recibo (ou `TOKEN_VAZIO_RECEIPT_NOT_CREATED`) no sumário
de Actions, **inclusive quando falhar**. Arquivos originais continuam imutáveis.

**Este documento é um contrato/estado de fonte, não uma prova de execução deste HEAD.**
