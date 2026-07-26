<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Contribuindo com o RMR Crypto Registry

## 1. Escopo permitido

Contribuições desta trilha devem permanecer em `rmr/crypto/`, salvo atualização documental mínima e explícita em:

- `rmr/readme.md`;
- `rmr/PROVENIENCE.md`;
- verificadores autorais já existentes.

Mudanças no core BLAKE3 (`src/`, `c/`, `reference_impl/`, `b3sum/`, `test_vectors/`) exigem PR independente e não podem ser misturadas com este módulo.

## 2. Tipos de contribuição

- correção do registro;
- prova de parentesco de fork;
- evidência de licença fixada em commit;
- novo teste ou validador offline;
- documentação de arquitetura, segurança ou custódia;
- adaptador RMR explicitamente isolado;
- correção de claim ou promoção de `TOKEN_VAZIO` com evidência.

Importação de código de terceiro não é contribuição comum; exige o processo reforçado da seção 6.

## 3. Fluxo obrigatório

1. Abra issue ou descreva o problema no PR.
2. Declare o escopo e os caminhos afetados.
3. Classifique cada mudança como `CODE`, `DOC`, `DATA`, `TEST`, `LEGAL_METADATA` ou `THIRD_PARTY_IMPORT`.
4. Inclua cabeçalho canônico, quando o formato permitir.
5. Atualize `rmr/PROVENIENCE.md` para novos artefatos.
6. Execute `./rmr/crypto/tools/audit_crypto_registry.sh`.
7. Anexe saída, ambiente e commit testado.
8. Preencha a atestação de contribuição.
9. Mantenha o PR em rascunho enquanto licença, autoria ou segurança estiverem em `TOKEN_VAZIO` material.
10. Exija revisão humana antes do merge.

## 4. Padrão de commits

```text
<tipo>(rmr-crypto): <ação objetiva>
```

Tipos recomendados:

- `docs`
- `feat`
- `fix`
- `test`
- `audit`
- `legal`
- `chore`

Exemplos:

```text
docs(rmr-crypto): registrar fronteira de autoria do SHA-256
audit(rmr-crypto): verificar licença do upstream em commit fixado
test(rmr-crypto): adicionar vetor conhecido SHA-256
```

## 5. Checklist de PR

- [ ] Mudança isolada do núcleo upstream.
- [ ] Cabeçalhos canônicos presentes ou exceção registrada.
- [ ] Origem de todo material declarada.
- [ ] Nenhum segredo, token, chave ou dado pessoal incluído.
- [ ] Registro permanece `reference_only` quando não há importação.
- [ ] Relação de fork não foi presumida pelo nome.
- [ ] Licença não foi promovida sem evidência no commit fixado.
- [ ] Claims têm estado, evidência e falsificador.
- [ ] KATs e auditor local passam.
- [ ] Logs e ambiente estão anexados.
- [ ] Assistência por IA foi declarada quando material.
- [ ] Revisão humana final foi realizada.

## 6. Importação de terceiro

Um PR `THIRD_PARTY_IMPORT` deve conter, no mínimo:

```yaml
source_repository: owner/repo
source_commit: <sha completo>
source_path: <caminho>
source_license_path: <caminho no commit>
source_notice_path: <caminho ou NONE>
source_raw_sha256: <digest>
imported_tree_sha256: <digest>
modifications_documented: true
license_compatibility: VERIFIED_BY_HUMAN | TOKEN_VAZIO
attribution_preserved: true
trademark_claim: NONE
```

Também deve:

- incluir cópia exigida da licença e notices;
- marcar arquivos alterados;
- preservar histórico quando viável;
- explicar link estático/dinâmico, distribuição e finalidade;
- impedir merge enquanto `license_compatibility=TOKEN_VAZIO`.

## 7. Promoção de estados

A promoção deve ser monotônica e justificável:

```text
TOKEN_VAZIO
  → DECLARED
  → DOCUMENTED
  → VERIFIED_PRIMARY ou VERIFIED_LOCAL
```

Uma evidência contrária pode rebaixar ou marcar `REFUTED_IN_SCOPE`. Histórico não deve ser apagado.

## 8. Segurança

Falhas sensíveis não devem ser publicadas inicialmente em issue aberta. Use o canal privado de segurança do GitHub quando disponível. Não inclua exploit funcional, segredo real ou dados de vítima em PR público.

## 9. Atestação

Inclua no corpo do PR:

```text
RMR-Contribution-Attestation: v1
Author: <nome/login>
Scope: <escopo>
Origin: <ORIGINAL|AUTHORIZED_DERIVATIVE|THIRD_PARTY_IMPORT>
Third-Party-Material: <NONE|lista>
License-Evidence: <referência|TOKEN_VAZIO>
AI-Assistance: <NONE|ferramenta e escopo>
Tests: <comandos e resultado>
Human-Reviewed: YES
Signed-off-by: <nome/login>
```

## 10. Critérios de rejeição

O PR deve ser bloqueado quando:

- mistura autoria upstream e RMR;
- altera licença por automação;
- importa código sem licença;
- declara certificação inexistente;
- apresenta benchmark sem equivalência de saída;
- troca `TOKEN_VAZIO` por certeza sem evidência;
- inclui material confidencial;
- modifica o núcleo criptográfico fora do escopo declarado.