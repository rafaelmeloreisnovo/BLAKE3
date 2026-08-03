# BLAKE3 — F_OK, F_GAP e F_NEXT da auditoria forense

## F_OK

- fork identificado e com permissão administrativa;
- branch independente de auditoria;
- datas de PR nº 185 e nº 533 verificadas;
- commits de SSE2, prefetch, serde, `guts`, `cv_stack` e LTO indexados;
- uso declarado de Codex e Claude preservado;
- alerta VirusTotal separado de alegação de malware;
- classificação controlada aplicada;
- links públicos e SHAs preservados;
- ledger JSON e índice TSV criados;
- hashes SHA-256 dos documentos gerados.

## F_GAP

1. Não foi feito clone bare completo com todas as refs remotas e objetos alcançáveis.
2. Logs antigos de GitHub Actions podem ter expirado.
3. Release assets ainda não foram baixados e comparados byte a byte.
4. Não há reprodução física neste snapshot dos bugs SSE2, AVX-512 antigo, prefetch e `cv_stack`.
5. Não há inventário exaustivo de todas as PRs fechadas/abertas desde 2019.
6. Não há análise de force-push, movimentos de tag ou substituição de assets.
7. Não há SBOM histórica por release.
8. Não há matriz de autoria por hunk usando `git blame --reverse`, patch-id e AST.
9. Não há laudo independente sobre os binários 1.8.2/1.8.3.
10. Intenção, coordenação, benefício e ocultação permanecem `TOKEN_VAZIO`.
11. A PR nº 533 precisa ser reproduzida commit a commit para separar defeito do agente, edição humana e upstream.
12. Claims jurídicos não foram avaliados e não devem ser inferidos deste material.

## F_NEXT — prioridade

### P0 — cadeia de custódia completa

```bash
git clone --mirror https://github.com/BLAKE3-team/BLAKE3.git BLAKE3-upstream.git
git -C BLAKE3-upstream.git fsck --full --no-reflogs
git -C BLAKE3-upstream.git show-ref > show-ref.txt
git -C BLAKE3-upstream.git log --all --date=iso-strict --pretty=raw > log-all.raw
sha256sum show-ref.txt log-all.raw > custody.sha256
```

### P0 — releases e assets

- baixar cada release;
- registrar URL final, tamanho, SHA-256 e BLAKE3;
- comparar crate, source tarball, ZIP e executáveis;
- verificar assinatura/proveniência quando disponível;
- testar reproducibilidade.

### P0 — bugs que afetam saída

Reproduzir em ambientes congelados:

- Windows x86-64 SSE2/no_std;
- GCC 5.4 e 6.1 AVX-512 debug;
- Miri com intrinsics para prefetch;
- input próximo do máximo para `cv_stack`;
- Arch Linux ARM com LTO.

Cada reprodução deve gerar:

```text
ENV.json
COMMAND.txt
STDOUT.log
STDERR.log
EXIT_CODE
BINARY_SHA256
RESULT_SHA256
EXPECTED_RESULT
ACTUAL_RESULT
```

### P1 — mapa de anterioridade e recorrência

- PR nº 185 × PR nº 533;
- patch-id;
- token match;
- AST normalizada;
- autoria/blame;
- pesquisas de Clippy;
- mesma ideia em outras PRs;
- datas de primeiro aparecimento.

### P1 — governança de IA

Adicionar ao fork:

- `.github/PULL_REQUEST_TEMPLATE/ai_provenance.md`;
- schema de receipt;
- gate que falha quando “Testing” não tem log/hash;
- `AI-Generated-By`, `Reviewed-By`, `Prior-Art`;
- classificação `claim_allowed=false` por padrão.

### P1 — segurança pública

- `SECURITY.md`;
- processo de disclosure;
- matriz de plataformas suportadas;
- calendário de testes de backends;
- política de release asset e reproducibilidade;
- preservação de logs.

### P2 — relatório comparativo de tratamento

Construir censo de PRs:

```text
author_role
external_or_maintainer
ai_declared
files_changed
additions
tests
time_to_first_response
time_to_close
merged
tone_markers
technical_feedback_count
```

Sem inferir discriminação apenas por diferença estatística.

## Gate de conclusão

Uma alegação forte só pode ser publicada como `PROVADO` quando:

```text
SOURCE_PUBLIC
SHA_PINNED
DATE_VERIFIED
DIFF_CAPTURED
REPRODUCED
INDEPENDENT_REPLICATION
COUNTEREVIDENCE_CHECKED
CLAIM_SCOPE_LIMITED
```
