# RMR/BLAKE3 — pesquisa de proveniência, custódia e retroalimentação

**Estado:** `DOCUMENTED_RESEARCH_DRAFT / HOLD / claim_allowed=false`  
**Base produtora:** `rafaelmeloreisnovo/BLAKE3@26daf57b951ce5003177eadf3e15f638930176ba`  
**Registro de abertura:** Supabase `raf_private.omega_continuity_events_v1`, evento `MU-RMR-BLAKE3-PROVENANCE-RESEARCH-20261105-S1-BEGIN`, seq. 62, SHA-256 `49a0330991f5eaee7b3cbd3e7b98d365ddb726bfb679d286c59d4e3ab8873346`.

## Resultado delimitado

A pesquisa confirma uma arquitetura de evidência, não uma execução do RMR:

```text
fonte exata
→ receita/ambiente de build
→ artefato identificado
→ digest
→ execução observada
→ receipt com predecessor
→ falsificador
→ verificação independente
→ claim estritamente limitado
```

O digest BLAKE3 pode provar igualdade/integridade dos bytes dentro do modelo criptográfico. Ele não prova sozinho autoria humana, completude da captura, correção do processo, autoridade, execução física ou verdade científica.

## Fontes alphaXiv e uso permitido

| ID | Contribuição útil | Fronteira |
|---|---|---|
| [2608.18180](https://www.alphaxiv.org/abs/2608.18180) | Reprodutibilidade não basta: a verificação exige recuperar source revision, ambiente, dependências e instruções. | Não verifica este repositório nem seus artefatos. |
| [2605.21089](https://www.alphaxiv.org/abs/2605.21089) | Protocolo de CI orientado a evidências, builds determinísticos e attestations. | Resultados TEE/Nix/TDX não transferem automaticamente ao GitHub Actions, Termux ou ARM. |
| [2605.08363](https://www.alphaxiv.org/abs/2605.08363) | Proveniência attestada liga commit, dependências, toolchain, ambiente e digest do artefato. | Nenhuma execução Kettle/TEE foi feita aqui. |
| [2602.01663](https://www.alphaxiv.org/abs/2602.01663) | Assinatura/digest não equivale a autoria ou integridade do processo. | O mecanismo proposto pelo paper não foi implementado no RMR. |
| [2610.37982](https://www.alphaxiv.org/abs/2610.37982) | Fontes autênticas podem ser recombinadas em claims não autorizados; qualificações e revisão precisam viajar com a evidência. | Não valida desempenho, autoria ou completude RMR. |
| [2605.08586](https://www.alphaxiv.org/abs/2605.08586) | Números experimentais devem estar ligados ao cálculo que os produziu por registro verificável. | Não prova que benchmark RMR algum foi executado. |
| [2609.01931](https://www.alphaxiv.org/abs/2609.01931) | Eventos canônicos, predecessor, intenção, execução, efeitos e proveniência fortalecem a auditoria. | Hashchain preserva o registrado; não garante completude nem verdade de eventos ausentes. |

## Invariante das invariantes

| Campo | Estado |
|---|---|
| Origem | repositório, commit e blobs fixados |
| Identidade | BLAKE3 upstream ≠ fork ≠ adaptador RMR ≠ artefato runtime |
| Distinção | SOURCE ≠ ARTIFACT ≠ EXECUTION ≠ EVIDENCE ≠ CLAIM |
| Predecessor | Supabase S1-BEGIN preservado |
| TOKEN_VAZIO | não convertido em zero, falso ou PASS |
| Evidência | literatura = suporte de arquitetura; execução local = NOT_RUN |
| Reversão | branch isolada; fechar ou reverter somente deltas aditivos |
| Promoção | bloqueada; `claim_allowed=false` |

## Gates seguintes

1. Executar vetores conhecidos BLAKE3 e negativos do adaptador RMR no HEAD exato.
2. Registrar comando, compilador, flags, target, binário, SHA-256/BLAKE3 do artefato, log e exit status.
3. Comparar ao upstream declarado sem alterar a lógica criptográfica upstream.
4. Reproduzir em host independente equivalente.
5. Separar cross-compile de execução física ARMv7/ARM64.
6. Revisar autoria e licença arquivo a arquivo; digest não substitui essa revisão.

## TOKEN_VAZIO preservado

- `TOKEN_VAZIO_RMR_BLAKE3_EXECUTION`
- `TOKEN_VAZIO_CURRENT_UPSTREAM_DIFF`
- `TOKEN_VAZIO_INDEPENDENT_REPRODUCTION`
- `TOKEN_VAZIO_PHYSICAL_ARMV7_ARM64`
- `TOKEN_VAZIO_AUTHORSHIP_BEYOND_REVIEWED_RMR_FILES`

## Rollback

Base preservada: `26daf57b951ce5003177eadf3e15f638930176ba`.  
Sem alteração do núcleo, sem merge, sem release e sem apagamento. Correção futura cria successor no Supabase e commit aditivo/superseding; não reescreve o S1.

## R3

- **F_ok:** autoridade, base, blobs, gate pré-pesquisa e sete fontes alphaXiv vinculados.
- **F_gap:** execução, rebuild independente, dispositivo físico e promoção autoral/científica não realizados.
- **F_next:** bind do commit documental ao successor Supabase; depois KAT/negativos em gate separado e revisão humana.
