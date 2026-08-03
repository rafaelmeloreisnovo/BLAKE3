# BLAKE3 — Matriz de erro, reversão, aborto e reconstrução

| ID | Origem | Data origem | Evento | Descoberta/correção | Tempo aproximado | Impacto demonstrado | Estado atual | Intenção |
|---|---|---:|---|---|---:|---|---|---|
| F-001 | `0cb4f6d` | 2019-12-07 | assert disparava incorretamente com `MAX_SIMD_DEGREE=1` | corrigido no próprio commit | n/d | falha de teste/configuração | corrigido | `TOKEN_VAZIO` |
| F-002 | `b8c33e1` | 2020-01-19 | prefetch com ponteiro formado via `.add()` | PR #507 / `89537d9`, 2025-08 | ~5,6 anos | UB detectado por Miri | corrigido para `.wrapping_add()` | `TOKEN_VAZIO` |
| F-003 | `67262df` + binding antigo | 2020-01 | crescimento de pilha não refletido em binding de teste | `6eebbbd`, 2026-03-06 | ~6 anos | layout incorreto em teste; fronteira quase máxima | 1728→1760 | `TOKEN_VAZIO` |
| F-004 | suporte SSE2 original / PR #110 | antes de 2021-11 | registrador callee-save sobrescrito no Windows | issue #206 / `371b548` | múltiplas versões | hash incorreto + UB | security fix 1.2.0 | `TOKEN_VAZIO` |
| F-005 | intrinsics AVX-512 | antes de 2022-11 | intrinsic quebrado em GCC 5.4/6.1 debug | issue #271 / `342f9f8` | n/d | saída incorreta | release 1.3.3 | `TOKEN_VAZIO` |
| F-006 | serialização `serde_bytes` | 2024-07 | mudança anunciada como compatível não era compatível em `bincode` | issue #414 / `43ce639` | curto | quebra de retrocompatibilidade | revertida em 1.5.3 | conhecimento prévio `TOKEN_VAZIO` |
| F-007 | `blake3_guts` | 2023–2024 | arquitetura experimental grande | removida por `9190916` | n/d | código e CI deixam o master | reconstruível pelo Git | ocultação estrita `REFUTADA` |
| F-008 | Codex PR #495 | 2025-07-13 | port AVX-512 Windows gerado automaticamente | teste local do mantenedor | 4 min | `STATUS_ACCESS_VIOLATION` | fechada, não mesclada | experimento declarado |
| F-009 | binário 1.8.2 | 2025-09 | 2/72 alertas VirusTotal | issue #517; 1.8.3 sem alerta | meses | alerta isolado | issue encerrado | malware `NÃO DEMONSTRADO` |
| F-010 | PR #533 | 2025-11-25 | 24 commits e alterações amplas | revisão Copilot + fechamento | 2h46 | PR não mesclada | preservada no GitHub/fork | apropriação `NÃO EVIDENCIADA` |
| F-011 | LTO C intrinsics | 2026-04 | build break em Arch Linux ARM | issue #550 / `299b1e2` | n/d | falha de build | LTO desabilitado + CI | `TOKEN_VAZIO` |

## Padrões transversais

### P1 — rotas não exercitadas

SSE2 Windows, intrinsics de prefetch e inputs quase máximos mostram que o código pode estar visível e ainda assim permanecer operacionalmente invisível porque a matriz usual não alcança a condição.

### P2 — duplicação específica por plataforma

C, Rust, assembly GNU/MSVC, intrinsics e dispatch multiplicam pontos que precisam permanecer semanticamente equivalentes.

### P3 — correção posterior não apaga exposição anterior

Uma reversão ou correção deve preservar:

```text
INTRO_SHA
FIRST_AFFECTED_RELEASE
LAST_AFFECTED_RELEASE
FIX_SHA
TEST_RECEIPT
REPRO_ENVIRONMENT
USER_IMPACT
```

### P4 — master não é arquivo histórico suficiente

- `blake3_guts` saiu da árvore;
- serialização foi revertida;
- PRs de IA e humanas foram fechadas;
- branches/testes experimentais revelaram bugs.

A auditoria correta percorre DAG Git, PRs, issues, releases e artefatos.

## Critério para alegar plantação deliberada

A alegação só pode sair de `TOKEN_VAZIO` com evidência cumulativa de:

1. ator introduziu o erro;
2. ator conhecia o efeito;
3. teste capaz de detectar foi removido ou deliberadamente evitado;
4. informação foi ocultada ou falsificada;
5. o mesmo ator obteve benefício;
6. existe repetição ou coordenação;
7. reprodução independente confirma a cadeia.

Nenhum dos casos acima fecha os sete elementos.
