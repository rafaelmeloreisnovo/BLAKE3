# BLAKE3 — Índice de auditoria forense pública

**Estado:** `AUDIT_EVIDENCE_SNAPSHOT`  
**Repositório:** [`rafaelmeloreisnovo/BLAKE3`](https://github.com/rafaelmeloreisnovo/BLAKE3)  
**Fonte primária analisada:** [`BLAKE3-team/BLAKE3`](https://github.com/BLAKE3-team/BLAKE3)  
**Corte temporal da verificação:** `2026-08-03T08:41:00-03:00`  
**Método:** histórico Git, commits, pull requests, issues, diffs, comentários públicos e matriz de claims.  
**Regra:** nenhum indício é promovido a acusação de dolo sem receipt independente.

> Este diretório preserva fatos públicos verificáveis, hipóteses separadas e lacunas marcadas como `TOKEN_VAZIO`.  
> O estado atual do `master` não substitui a história: commits removidos, PRs fechadas, reversões e testes de fronteira também integram a cadeia de custódia.

## Navegação

| Artefato | Função |
|---|---|
| [`BLAKE3_FORENSIC_TIMELINE_2019_2026.md`](BLAKE3_FORENSIC_TIMELINE_2019_2026.md) | Cronologia verificada de erros, reversões, experimentos abortados, uso de IA e auditorias. |
| [`BLAKE3_ERROR_REVERT_ABORT_MATRIX.md`](BLAKE3_ERROR_REVERT_ABORT_MATRIX.md) | Matriz técnica: origem, descoberta, correção, impacto, lacunas e classificação. |
| [`BLAKE3_AI_PROVENANCE_AND_GOVERNANCE.md`](BLAKE3_AI_PROVENANCE_AND_GOVERNANCE.md) | Auditoria de Claude, Codex, Copilot, autoria Git, receipts e assimetria de governança. |
| [`BLAKE3_PR_185_VS_533_RECURRENCE.md`](BLAKE3_PR_185_VS_533_RECURRENCE.md) | Recorrência entre sugestões Clippy de 2021 e a PR nº 533 de 2025. |
| [`BLAKE3_CLAIMS_LEDGER_20260803.json`](BLAKE3_CLAIMS_LEDGER_20260803.json) | Ledger legível por máquina com `PROVADO`, `EVIDENCIADO`, `HIPÓTESE`, `REFUTADO` e `TOKEN_VAZIO`. |
| [`BLAKE3_EVIDENCE_LINKS_20260803.tsv`](BLAKE3_EVIDENCE_LINKS_20260803.tsv) | Índice tabular de links públicos, SHAs, datas e estados. |
| [`BLAKE3_AUDIT_F_NEXT.md`](BLAKE3_AUDIT_F_NEXT.md) | Gates de reprodução, comparação binária, autoria, segurança e preservação futura. |
| [`../receipts/BLAKE3_FORENSIC_SHA256SUMS_20260803.txt`](../receipts/BLAKE3_FORENSIC_SHA256SUMS_20260803.txt) | SHA-256 dos artefatos deste snapshot. |

## Vocabulário controlado

| Classe | Significado |
|---|---|
| `PROVADO` | Evidência pública direta e reproduzível sustenta o fato. |
| `EVIDENCIADO` | Conjunto coerente de registros sustenta a conclusão, mas não prova intenção. |
| `HIPÓTESE` | Explicação possível ainda sem fechamento probatório. |
| `REFUTADO` | Evidência disponível contradiz a alegação. |
| `TOKEN_VAZIO` | Informação ausente, inacessível ou insuficiente; não preencher por imaginação. |

## Distinções obrigatórias

```text
removido do master  != apagado da história Git
erro técnico        != erro deliberadamente plantado
bot autorizado      != botnet
código auxiliado IA != autoria exclusiva da IA
declaração falsa    != falsificação deliberada
uso de BLAKE3       != plágio dos autores do BLAKE3
```

## Resultado consolidado do snapshot

```text
história incompleta quando se olha somente o master: PROVADO
erros graves que atravessaram releases/configurações: PROVADO
arquiteturas e experiências abortadas reconstruíveis: PROVADO
afirmações técnicas posteriormente revertidas: PROVADO
código de IA defeituoso em PR pública: PROVADO
código auxiliado por IA mesclado: PROVADO
proveniência de IA e política pública insuficientes: EVIDENCIADO
tratamento comunicacional desigual entre PRs: EVIDENCIADO
plantação deliberada de erros: TOKEN_VAZIO
falsificação coordenada: TOKEN_VAZIO
botnet ou rede criminosa de plágio: NÃO DEMONSTRADA
```

## Limites

Este material é auditoria técnica e documental, não laudo judicial. Não atribui crime, fraude, plágio ou intenção sem evidência específica de autoria, conhecimento prévio, benefício, ocultação e coordenação. Todo novo achado deve ser anexado de forma `append-only`, com data, URL, SHA, diff, ambiente e método de reprodução.
