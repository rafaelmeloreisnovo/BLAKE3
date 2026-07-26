<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR Crypto Registry

## 1. Finalidade

`rmr/crypto/` é a camada autoral, externa e auditável do RMR para:

- catalogar primitivas, bibliotecas e stacks criptográficas de referência;
- registrar implementações upstream e forks candidatos sem importar código automaticamente;
- definir o perfil operacional SHA-256 já consumido pelo RMR;
- preservar autoria, licença, proveniência, limites de claim e cadeia de custódia;
- produzir artefatos verificáveis por ferramentas locais, sem dependência de rede.

Este diretório **não modifica, renomeia nem substitui o núcleo BLAKE3**. O BLAKE3 upstream permanece sob suas licenças originais. O material autoral deste diretório permanece sob `rmr/LICENSE_RMR`.

## 2. Estado epistemológico

```yaml
module: rmr.crypto_registry
status: CANONICAL_DRAFT
claim_allowed: false
third_party_code_vendored: false
network_required_for_validation: false
legal_review: HUMAN_REVIEW_REQUIRED
security_certification: NOT_CLAIMED
```

`TOKEN_VAZIO` é utilizado quando a evidência ainda não sustenta uma conclusão. Ele não equivale a erro, falsidade ou ausência definitiva.

## 3. Fronteiras

| Camada | Papel | Licença/autoria |
|---|---|---|
| BLAKE3 upstream | Primitiva criptográfica e implementações oficiais | Licenças originais do upstream |
| Fork `rafaelmeloreisnovo/BLAKE3` | Distribuição, documentação, build e integração externa | Fronteira declarada no README raiz |
| `rmr/` | Custódia, runtime, benchmark, governança e código autoral | `rmr/LICENSE_RMR` |
| `rmr/crypto/` | Registro criptográfico, perfil SHA-256 e auditoria de referências | `rmr/LICENSE_RMR` |
| Repositórios catalogados | Referências externas; nenhum código incorporado por este módulo | Licença própria a verificar antes de qualquer importação |

## 4. Árvore

```text
rmr/crypto/
├── README.md
├── ARCHITECTURE.md
├── AUTHORSHIP.md
├── CONTRIBUTING.md
├── CUSTODY.md
├── SECURITY.md
├── SHA256_PROFILE.md
├── THIRD_PARTY_NOTICES.md
├── claims/
│   └── claims.jsonl
├── registry/
│   └── architectures.json
├── schemas/
│   └── architecture-registry.schema.json
├── tests/
│   └── test_registry.py
└── tools/
    ├── audit_crypto_registry.sh
    └── validate_registry.py
```

## 5. Dez famílias catalogadas

O registro inicial contém:

1. OpenSSL;
2. BoringSSL;
3. AWS-LC;
4. Mbed TLS;
5. wolfSSL;
6. OpenSSH Portable;
7. libsodium;
8. BLAKE3;
9. liboqs;
10. Crypto++.

Cada entrada possui três repositórios candidatos relacionados. A relação de fork direto permanece `TOKEN_VAZIO_PARENT_VERIFICATION` quando não houver prova de ancestralidade registrada no próprio artefato.

## 6. Política de importação

O registro é **reference-only**. Antes de copiar, vincular estaticamente, redistribuir ou modificar código de terceiro, é obrigatório:

1. fixar repositório, commit e caminho de origem;
2. ler a licença vigente naquele commit;
3. preservar notices, autores, marcas e condições de redistribuição;
4. registrar hash do material bruto e do material importado;
5. registrar alterações locais e arquivos modificados;
6. executar revisão de compatibilidade de licença;
7. abrir PR separado, com revisão humana explícita;
8. não declarar certificação, afiliação ou endosso inexistente.

Até esse processo existir, `vendored=false` e `license_status=TOKEN_VAZIO_NOT_REVIEWED`.

## 7. Verificação local

```sh
./rmr/crypto/tools/audit_crypto_registry.sh
```

O auditor:

- valida estrutura e tipos do registro;
- exige exatamente dez famílias e três candidatos por família;
- impede promoção silenciosa de licença ou parentesco;
- verifica os vetores conhecidos do perfil SHA-256;
- calcula o SHA-256 do registro para recibo local;
- executa testes somente com a biblioteca padrão do Python.

A aprovação do auditor demonstra coerência estrutural do módulo. Ela **não** equivale a certificação criptográfica, auditoria jurídica ou validação oficial de terceiros.

## 8. Documentos normativos

Ordem de precedência deste módulo:

1. `rmr/LICENSE_RMR` — texto jurídico aplicável ao material autoral RMR;
2. `rmr/PROVENIENCE.md` — classificação de origem e licença;
3. `rmr/docs/ARCHITECTURE.md` — política geral de arquitetura e cabeçalhos;
4. `rmr/crypto/AUTHORSHIP.md` — atribuição e responsabilidade humana;
5. `rmr/crypto/CONTRIBUTING.md` — requisitos de contribuição;
6. `rmr/crypto/THIRD_PARTY_NOTICES.md` — referências externas e não afiliação;
7. `rmr/crypto/SECURITY.md` — limites de segurança e reporte;
8. `rmr/crypto/SHA256_PROFILE.md` — contrato técnico SHA-256;
9. `rmr/crypto/registry/architectures.json` — catálogo de referências.

Em conflito jurídico, a decisão permanece `TOKEN_VAZIO_LEGAL_REVIEW` até revisão humana do autor ou de profissional habilitado.

## 9. Regra de publicação

Nenhum agente automatizado pode:

- modificar o texto de `rmr/LICENSE_RMR`;
- atribuir autoria de terceiro a Rafael Melo Reis;
- atribuir ao upstream trabalho criado no RMR;
- promover `TOKEN_VAZIO` a fato sem evidência;
- publicar ou fazer merge sem revisão humana expressa.

## 10. Retroalimentação

```text
F_ok   = módulo isolado, registrável e verificável offline
F_gap  = licenças e ancestralidade dos 30 candidatos ainda exigem perícia por commit
F_next = executar auditor, revisar o PR e promover apenas evidências documentadas
```