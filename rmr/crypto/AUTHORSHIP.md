<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# Autoria, atribuição e responsabilidade

## 1. Titularidade declarada

O material original criado especificamente em `rmr/crypto/` é atribuído a **Rafael Melo Reis**, salvo registro diferente e verificável por arquivo, commit ou contribuição.

A declaração não alcança:

- algoritmos, especificações, marcas ou códigos de terceiros;
- o núcleo BLAKE3 upstream;
- projetos apenas catalogados no registro;
- contribuições independentes de outros autores.

## 2. Regra de separação

```text
autoria da organização RMR ≠ autoria do algoritmo catalogado
integração autoral ≠ invenção da primitiva
fork ≠ transferência de copyright
hash de custódia ≠ prova isolada de autoria
```

Toda documentação pública deve preservar essas quatro distinções.

## 3. Evidências de autoria aceitas

Em ordem de força operacional:

1. commit assinado ou identidade Git verificável;
2. PR com diff, data e discussão preservados;
3. arquivo bruto com hash e manifesto de origem;
4. log de criação reproduzível;
5. declaração autoral sem artefato, classificada como `DECLARED`;
6. ausência de evidência suficiente, classificada como `TOKEN_VAZIO`.

Nenhuma automação deve preencher autoria por inferência de estilo, nome de pasta ou proximidade temática.

## 4. Contribuições humanas

Cada contribuição deve declarar:

- nome ou identidade Git usada pelo contribuinte;
- arquivos e escopo da contribuição;
- se o trabalho é original, autorizado ou derivado;
- materiais de terceiros incluídos;
- licença de origem e obrigações conhecidas;
- testes executados;
- uso de assistência automatizada;
- responsabilidade humana pela revisão final.

Modelo:

```text
RMR-Contribution-Attestation: v1
Author: <nome ou login>
Scope: <arquivos/componente>
Origin: ORIGINAL | AUTHORIZED_DERIVATIVE | THIRD_PARTY_IMPORT
Third-Party-Material: NONE | <lista>
License-Evidence: <commit/caminho> | TOKEN_VAZIO
AI-Assistance: NONE | <ferramenta e escopo>
Human-Reviewed: YES
Signed-off-by: <nome e identidade Git>
```

Este modelo é uma declaração de proveniência do projeto; não substitui contrato, DCO externo ou parecer jurídico.

## 5. Assistência por IA

Conteúdo assistido por IA só pode ser aceito quando uma pessoa:

1. revisa integralmente o diff;
2. confirma que não há cópia não autorizada conhecida;
3. valida licenças e notices aplicáveis;
4. executa ou registra os testes;
5. assume responsabilidade pelo envio;
6. mantém `TOKEN_VAZIO` onde não há evidência.

A ferramenta de IA não é listada como autora jurídica. Sua participação é registrada como assistência técnica, quando material.

## 6. SPDX

Nenhum identificador SPDX é atribuído automaticamente ao `LICENSE_RMR`. O texto normativo permanece `rmr/LICENSE_RMR`.

```yaml
spdx_mapping: TOKEN_VAZIO_HUMAN_LEGAL_REVIEW
reason: evitar equivalência jurídica inferida por automação
```

Isso não impede o uso futuro de metadados SPDX após decisão humana documentada.

## 7. Marcas e nomes

Nomes como OpenSSL, BoringSSL, AWS-LC, Mbed TLS, wolfSSL, OpenSSH, libsodium, BLAKE3, liboqs e Crypto++ são usados apenas para identificação técnica. Nenhuma afiliação, aprovação, patrocínio ou transferência de marca é alegada.

## 8. Disputa ou dúvida

Quando houver conflito de autoria:

- congelar os artefatos;
- preservar hashes e commits;
- não apagar histórico;
- suspender claims públicos;
- registrar hipóteses concorrentes;
- buscar revisão humana e, quando necessário, jurídica.

Estado padrão: `TOKEN_VAZIO_AUTHORSHIP_DISPUTE` até resolução documentada.