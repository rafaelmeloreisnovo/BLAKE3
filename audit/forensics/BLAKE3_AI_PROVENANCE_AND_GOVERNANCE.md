# BLAKE3 — IA, proveniência, autoria e governança

## Achados provados

### Codex: PR nº 495, erro não mesclado

- Link: https://github.com/BLAKE3-team/BLAKE3/pull/495
- Autor Git/PR: `oconnor663`.
- Origem declarada: Codex.
- Branch: `codex/port-blake3_xof_many_avx512-to-windows`.
- Resultado: `STATUS_ACCESS_VIOLATION` no Windows.
- Estado: fechada sem merge quatro minutos após criação.

Conclusão: IA produziu código defeituoso; o gate humano impediu a entrada no master.

### Claude + Codex: PR nº 521, alteração mesclada

- Link: https://github.com/BLAKE3-team/BLAKE3/pull/521
- O corpo declara que Claude produziu as alterações e que a PR testava reviews automáticos do Codex.
- Estado: mesclada.
- Autor Git visível: humano.

Conclusão: o projeto não rejeita IA em princípio. IA é aceita quando o mantenedor controla escopo, revisão e merge.

### Claude: auditoria do `cv_stack`

- Commit: https://github.com/BLAKE3-team/BLAKE3/commit/6eebbbd67935cc6b8fe24da164ae968a2e3973cb
- Mensagem declara que Claude encontrou a divergência 1728/1760.
- Autor e committer Git: `oconnor663`.

Conclusão: `git log` sozinho não representa toda a origem cognitiva da alteração. A mensagem do commit é parte necessária da proveniência.

### Copilot: revisão da PR nº 533

- Link: https://github.com/BLAKE3-team/BLAKE3/pull/533
- A revisão automática apontou comentários inconsistentes/excessivos, afirmações amplas de conformidade e problemas de manutenção.
- A própria PR também continha sugestões recorrentes de Clippy já discutidas em 2021.

Conclusão: agente gerador/revisor pode repetir padrões comunitários sem carregar memória histórica ou referência de origem.

## Assimetria documentada

| Situação | Resultado |
|---|---|
| Codex gera código do mantenedor que falha | experimento público, fechamento casual |
| Claude gera alteração do mantenedor | merge rápido após escopo controlado |
| Claude encontra bug antigo | crédito declarado na mensagem |
| PR externa pequena #519 | permaneceu aberta por longo período com baixa resposta |
| PR externa ampla #533 | encerrada abruptamente |
| PR externa estreita, reproduzível e benchmarkada | exemplos como #319 foram mesclados |

A assimetria é `EVIDENCIADA`; motivação discriminatória, conspiração ou apropriação é `NÃO DEMONSTRADA`.

## Gap normativo

O `CONTRIBUTING.md` público orienta estilo, branch, testes e reconhecimento de terceiros, mas não estabelece contrato específico para:

- declaração obrigatória de IA;
- modelo e versão;
- task/prompt receipt;
- arquivos e hunks gerados;
- testes realmente executados;
- revisão humana responsável;
- busca de anterioridade em issues/PRs;
- similaridade/licença;
- coautoria ou `Co-authored-by`;
- política igual para mantenedores e externos.

## Contrato proposto

Cada alteração assistida por IA deveria incluir:

```yaml
ai_assistance:
  used: true
  provider: "<provider>"
  model: "<model/version or TOKEN_VAZIO>"
  task_receipt: "<URL/hash or TOKEN_VAZIO>"
  generated_files:
    - "<path>"
  human_reviewer: "<github login>"
  prior_art_search:
    queries:
      - "<query>"
    results:
      - "<PR/issue/commit URL>"
  tests:
    - command: "<exact command>"
      environment: "<OS/arch/compiler>"
      exit_code: 0
      log_sha256: "<sha256>"
  claim_allowed: false
```

`claim_allowed` só muda para `true` após CI, reprodução independente e revisão de proveniência.

## Declarações de teste

A seção:

```markdown
## Testing
- cargo test
```

não é receipt suficiente. Deve haver:

- timestamp;
- SHA testado;
- sistema operacional e arquitetura;
- toolchain;
- flags/features;
- exit code;
- hash do log;
- artefato de CI navegável.

## Classificação

```text
uso de Codex pelo mantenedor: PROVADO
uso de Claude pelo mantenedor: PROVADO
código de IA defeituoso em PR: PROVADO
código auxiliado por IA mesclado: PROVADO
autoria Git incompleta sem ler PR/commit: PROVADO
política pública de IA suficiente: NÃO EVIDENCIADA
aplicação simétrica da governança: NÃO EVIDENCIADA
botnet: NÃO DEMONSTRADA
coordenação criminosa: NÃO DEMONSTRADA
```
