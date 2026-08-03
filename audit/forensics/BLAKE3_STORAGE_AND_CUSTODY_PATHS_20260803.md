# BLAKE3 — Política de caminhos, gravação e cadeia de custódia

**Estado:** `STORAGE_ROUTE_POLICY`  
**Gerado em:** `2026-08-03T10:04:00-03:00`  
**Princípio:** cada tipo de evidência possui um lugar canônico; arquivos brutos são append-only.

## Árvore canônica

```text
audit/
├── raw/
│   ├── git/
│   │   ├── refs/
│   │   ├── objects/
│   │   ├── logs/
│   │   ├── fsck/
│   │   └── patches/
│   ├── github/
│   │   ├── prs/
│   │   ├── issues/
│   │   ├── comments/
│   │   ├── reviews/
│   │   ├── events/
│   │   └── actions/
│   ├── releases/
│   │   ├── metadata/
│   │   ├── assets/
│   │   ├── sbom/
│   │   └── attestations/
│   └── external/
│       ├── archives/
│       ├── antivirus/
│       └── receipts/
├── normalized/
│   ├── events/
│   ├── commits/
│   ├── refs/
│   ├── identities/
│   ├── conversations/
│   ├── executions/
│   └── artifacts/
├── forensics/
│   ├── timelines/
│   ├── matrices/
│   ├── contradictions/
│   ├── route_maps/
│   └── reports/
├── schemas/
├── templates/
├── receipts/
└── manifests/
tools/
└── forensics/
```

## Onde gravar cada coisa

| Evidência | Caminho |
|---|---|
| `show-ref`, tags e pull refs | `audit/raw/git/refs/<snapshot>/` |
| `git log --pretty=raw` | `audit/raw/git/logs/<snapshot>/` |
| objetos unreachable e `fsck` | `audit/raw/git/fsck/<snapshot>/` |
| patches e `range-diff` | `audit/raw/git/patches/<case_id>/` |
| JSON bruto de PR/issue | `audit/raw/github/prs/` ou `issues/` |
| comentários, reviews e edição | `audit/raw/github/comments/` e `reviews/` |
| Actions logs e artifacts | `audit/raw/github/actions/<run_id>/` |
| release metadata | `audit/raw/releases/metadata/<tag>/` |
| binários e source archives | `audit/raw/releases/assets/<tag>/` |
| VirusTotal/antivírus | `audit/raw/external/antivirus/<artifact_sha>/` |
| evento normalizado | `audit/normalized/events/YYYY/MM/` |
| identidade original/canônica | `audit/normalized/identities/` |
| contradição | `audit/forensics/contradictions/<case_id>.md` |
| claim | ledger JSON append-only em `audit/forensics/` |
| hashes | `audit/receipts/` |
| manifesto de snapshot | `audit/manifests/<snapshot>.json` |
| scripts | `tools/forensics/` |

## Nome canônico

```text
<UTC>__<SOURCE>__<KIND>__<IDENTIFIER>__<SHA12>.<ext>
```

Exemplo:

```text
20251125T004222Z__github__pr__533__15829f851e45.json
```

## Fluxo de gravação

1. Capturar bytes brutos sem editar.
2. Calcular SHA-256 e, quando disponível, BLAKE3.
3. Registrar URL, horário local, UTC, comando e ferramenta.
4. Gerar evento normalizado sem substituir o bruto.
5. Correlacionar por SHA, ref, actor, data e URL.
6. Criar claim com classificação controlada.
7. Registrar contraevidência e limite.
8. Fechar manifesto do snapshot.

## Regras de preservação

- nunca reusar nome de snapshot;
- nunca editar arquivo em `audit/raw/`;
- correções viram novo evento com `supersedes`;
- datas originais e UTC devem coexistir;
- autor, committer, pusher e agente são campos separados;
- hash de arquivo não substitui assinatura de identidade;
- ausência de dado é `TOKEN_VAZIO`, não conclusão;
- secrets, tokens e URLs autenticadas devem ser redigidos antes do commit;
- binários grandes podem ficar fora do Git, mas o manifesto, tamanho, URL e hashes devem entrar.

## Rotas físicas sugeridas no Termux

```text
$HOME/RAFAELIA/BLAKE3_AUDIT/                   trabalho local
$HOME/RAFAELIA/BLAKE3_AUDIT/mirror/            clone bare/mirror
$HOME/storage/shared/RAFAELIA_AUDIT/BLAKE3/    cópia navegável Android
Google Drive/00_CANONE/.../BLAKE3_AUDIT/        réplica externa
GitHub: audit/...                               índice, schemas, receipts e relatórios
```

A réplica no Drive deve preservar o mesmo `snapshot_id` e manifesto; não registrar no GitHub links privados ou credenciais.
