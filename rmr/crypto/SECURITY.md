<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Política de segurança — RMR Crypto Registry

## 1. Escopo

Esta política cobre código, dados, documentos e ferramentas sob `rmr/crypto/`. O catálogo não certifica os projetos externos relacionados.

## 2. Reporte

- Vulnerabilidade sensível: usar o canal privado de Security Advisories do GitHub, quando habilitado.
- Problema documental sem impacto sensível: issue pública.
- Não publicar inicialmente chaves, tokens, dados pessoais, exploit operacional ou informação de vítima.

Nenhum endereço de e-mail é presumido neste documento.

## 3. Classificação

| Severidade | Exemplo |
|---|---|
| Crítica | corrupção de digest aceita como válida; execução arbitrária no auditor |
| Alta | path traversal; importação silenciosa de código não autorizado |
| Média | manifesto ambíguo; relação de fork promovida sem prova |
| Baixa | erro de documentação sem efeito no resultado |

## 4. Regras criptográficas

- Não criar primitivas próprias nesta trilha.
- Não usar MD5 ou SHA-1 como prova adversarial principal.
- Não usar SHA-256 puro para senha.
- Não tratar hash como assinatura digital.
- Não comparar digests com semântica diferente como se fossem equivalentes.
- Não declarar conformidade FIPS/ISO/Common Criteria sem certificado verificável.
- Não usar benchmark como prova de segurança.

## 5. Segurança da cadeia de custódia

Um recibo deve incluir, quando disponível:

- algoritmo e versão;
- bytes ou canonicalização da entrada;
- digest;
- commit/ref;
- ambiente e ferramenta;
- timestamp UTC e origem do relógio;
- caminho do artefato;
- identidade do operador;
- status da verificação.

Se o agente que calcula o digest também puder reescrever silenciosamente o manifesto, integridade forte exige assinatura ou armazenamento externo imutável. Essa proteção permanece fora do escopo inicial.

## 6. Supply chain

Antes de importar terceiro:

- fixar SHA completo;
- verificar licença e notices no SHA fixado;
- evitar branch mutável como evidência;
- calcular hash do material bruto;
- registrar patch local;
- executar análise de dependências;
- revisar scripts de build e workflows;
- bloquear download automático não pinado.

## 7. Segredos e privacidade

É proibido versionar:

- chaves privadas;
- tokens de acesso;
- cookies ou sessões;
- dumps com credenciais;
- número de série bruto quando identificável e desnecessário;
- dados pessoais sem base, minimização e finalidade documentadas.

Fixtures devem ser sintéticas ou anonimizadas de modo verificável.

## 8. Estados de segurança

- `STRUCTURALLY_VERIFIED`: estrutura e testes locais passam.
- `CRYPTOGRAPHICALLY_VALIDATED`: somente com evidência formal apropriada.
- `PRODUCTION_REVIEWED`: revisão de ameaça, integração e operação concluída.
- `TOKEN_VAZIO_SECURITY_REVIEW`: evidência insuficiente.

O estado inicial deste módulo é `STRUCTURALLY_VERIFIED_PENDING_EXECUTION`, nunca `CRYPTOGRAPHICALLY_VALIDATED`.

## 9. Resposta

1. preservar o relatório original;
2. reproduzir em ambiente controlado;
3. classificar impacto e escopo;
4. preparar correção e teste de regressão;
5. revisar autoria/licença do patch;
6. publicar linguagem limitada à evidência;
7. registrar hash, commit e linha do tempo;
8. revisar claims anteriores afetados.