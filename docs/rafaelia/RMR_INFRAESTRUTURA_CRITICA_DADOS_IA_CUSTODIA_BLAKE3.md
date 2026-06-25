# RMR/BLAKE3 — infraestrutura crítica, dados, IA e cadeia de custódia

> Documento RAFAELIA/RMR para posicionar BLAKE3 como primitivo de integridade dentro de uma arquitetura maior de prova, auditoria, governança de dados, infraestrutura crítica, plataformas digitais e rastreabilidade.

## 1. Encaixe no fork BLAKE3

Este repositório mantém BLAKE3 como primitivo criptográfico. A camada RMR/RAFAELIA não substitui o BLAKE3, não altera sua semântica criptográfica e não deve afirmar superioridade genérica sobre o upstream.

O papel correto desta camada é:

```text
BLAKE3 = primitivo de integridade
RMR/RAFAELIA = cadeia de custódia + metadados + hashchain + auditoria + contexto de prova
Fiber H = método autoral separado, dependente de spec e benchmark próprios
```

Ver também: [`RMR_LICENSE_FIBER_H_BOUNDARY.md`](./RMR_LICENSE_FIBER_H_BOUNDARY.md).

## 2. Tese central

Quando uma infraestrutura digital concentra dados, mapas, anúncios, IA, recomendação, marketplace, pagamentos, comportamento social e conteúdo sensível, o controlador deve demonstrar governança, rastreabilidade, auditoria, mitigação de risco e conformidade.

Reputação, escala e utilidade pública não bastam.

```text
Governança_real = documentação + logs + auditoria + rastreabilidade + mitigação + responsável + prova_de_conformidade
```

## 3. Dividir para conquistar como arquitetura de opacidade

A fragmentação empresarial e técnica pode ser legítima. O risco nasce quando ela dilui responsabilidade e torna a origem de dados, decisões e receitas difícil de reconstruir.

```text
Blindagem_operacional = produto + afiliadas + termos + APIs + SDKs + adtech + cloud + pagamento + jurisdição + terceiros
```

| Fragmentação | Risco de governança |
|---|---|
| Produto separado de pagamento | ninguém assume a monetização inteira |
| Dados separados de anúncio | ninguém assume o perfil completo |
| Plataforma separada de conteúdo | ninguém assume recomendação, ranking e retenção |
| IA separada de treinamento | ninguém explica origem, uso de dados e dependências |
| API separada de cliente | ninguém mostra custo real, social e regulatório |
| Afiliada local separada da matriz | dificulta responsabilidade territorial |
| Terceiros separados da empresa principal | cria zona cinzenta de controle e prova |

## 4. Origem indeterminável dos dados

Em ecossistemas de dados, a origem pode se perder por múltiplas camadas técnicas e contratuais:

```text
app -> SDK -> broker -> ad exchange -> anunciante -> medidor -> enriquecedor -> novo perfil
```

Depois aparece SMS, anúncio, recomendação, ligação, oferta ou sugestão comportamental, e o titular não consegue reconstruir facilmente a origem.

Formulação de governança:

```text
Se origem_do_dado = indeterminável_para_o_titular
então obrigação_de_rastreabilidade_do_controlador deve aumentar
```

## 5. Mapas/GPS, Waze, APIs e mobilidade

Mapas digitais não são apenas aplicativos de conveniência. Eles combinam localização, mobilidade, tráfego, API, dados derivados, anúncios, logística, rotas, serviços locais, cobrança por uso e infraestrutura crítica.

Modelo:

```text
Mapas_infra = GPS + localização + tráfego + APIs + anúncios + dados_derivados + cobrança_por_evento + dependência_social
```

Pontos auditáveis:

- quais dados são coletados;
- qual base legal é usada;
- que sinais alimentam rota, ranking e recomendação;
- como APIs são precificadas;
- como clientes e usuários são informados;
- como logs são preservados;
- como o usuário contesta ou audita inferências.

## 6. Algoritmo como dissolução de autoria

A frase “foi o algoritmo” não pode encerrar responsabilidade. Um algoritmo auditável precisa ter documentação suficiente para reconstruir decisão, objetivo, dados usados e mitigação de risco.

```text
Algoritmo_auditável = objetivo + dados_de_entrada + parâmetros + logs + testes + métricas + mitigação + responsável
```

Sem isso, “foi o algoritmo” equivale a uma caixa-preta operacional.

## 7. Penal vs governança: ônus correto

| Campo | Ônus adequado |
|---|---|
| Crime penal específico | acusação precisa provar fato, autoria, dolo/culpa, nexo e tipificação |
| LGPD/GDPR | controlador precisa demonstrar base legal, finalidade, transparência, segurança, minimização e conformidade |
| DSA/Big Tech | plataformas grandes precisam avaliar riscos, mitigar, documentar, permitir denúncia, explicar anúncios/recomendações e aceitar fiscalização |
| Mercado financeiro | agentes relevantes precisam reportar, auditar, controlar risco, documentar decisões e conflitos |
| Ambiental/ISO | licenças, auditorias e certificações demonstram conformidade |
| Conteúdo sensível envolvendo menores | após notícia formal, cresce dever de triagem, preservação, revisão, bloqueio/remoção se ilícito e encaminhamento |

Formulação segura:

> No penal, prova não se inverte por indignação. Mas em governança de dados, plataformas, anúncios, proteção infantil e infraestrutura crítica, a entidade controladora tem dever de demonstrar conformidade.

## 8. Certificação digital como âncora de prova

A certificação digital, os hashes, os logs e os commits não “criam” a verdade do fato. Eles provam que determinado artefato existia em determinada forma, em determinado contexto de cadeia de custódia.

```text
Certificação/Hash = prova de integridade + temporalidade + autoria operacional
Não = prova automática de veracidade material
```

Para RMR/RAFAELIA, a função é:

```text
metadado -> normalização -> BLAKE3 digest -> hashchain -> assinatura/contexto -> reprodutibilidade
```

## 9. BLAKE3 como camada de prova

BLAKE3 entra como ferramenta para tornar a auditoria reproduzível e eficiente.

### 9.1 Manifesto mínimo de evidência

```json
{
  "case_id": "TOKEN_VAZIO",
  "artifact_type": "document|log|metadata|protocol|source_snapshot",
  "source_url": "TOKEN_VAZIO",
  "captured_at": "TOKEN_VAZIO",
  "sha256": "TOKEN_VAZIO",
  "blake3": "TOKEN_VAZIO",
  "custody_actor": "TOKEN_VAZIO",
  "method": "metadata_only_no_sensitive_media",
  "notes": "TOKEN_VAZIO"
}
```

### 9.2 Regras

- Hash não prova verdade do conteúdo; prova integridade do artefato preservado.
- Hash sem cadeia de custódia é apenas identificador técnico.
- Cadeia de custódia exige data, origem, método, responsável, versão e contexto.
- Em conteúdo sensível, preservar metadados e protocolos, não material sensível.

## 10. Aplicação a plataforma e obra sensível

```text
Obra_sensível
+ catálogo_BigTech
+ classificação
+ monetização
+ busca
+ recomendação
+ dados_de_usuário
+ proteção_infantil
= dever_reforçado_de_auditoria
```

Perguntas de governança:

- quem aprovou o catálogo;
- qual classificação foi usada;
- se houve revisão de versão;
- se há recomendação para menores;
- se há anúncio ou remarketing;
- quais dados alimentam ranking;
- quais logs existem;
- qual órgão recebeu ciência;
- qual foi a resposta.

## 11. NetBus como analogia técnica

A mesma arquitetura técnica pode ser apresentada como administração, automação ou conveniência, mas produzir abuso se faltar governança, consentimento, auditoria e responsabilização.

```text
Ferramenta_legítima + ausência_de_governança = superfície_de_abuso
```

Essa lição é aplicável a IA, adtech, SDK, tracking, mapas e sistemas de recomendação.

## 12. IBM, Watson, quantum e infraestrutura profunda

IBM é uma infraestrutura histórica profunda de computação empresarial, IA corporativa, pesquisa, patentes, mainframe e computação quântica. Google, OpenAI, Microsoft, Meta, Apple, Amazon e outras empresas são camadas comerciais e operacionais reais do ecossistema contemporâneo.

O ponto auditável não é declarar “fachada”. O ponto auditável é exigir rastreabilidade de:

- infraestrutura;
- dados;
- modelos;
- dependências;
- cloud;
- hardware;
- contratos;
- governança;
- logs;
- avaliação de risco.

## 13. BlackRock/Aladdin e risco sistêmico

Sistemas que unificam risco, portfólio, dados, compliance, operações, trading, accounting e decisão de investimento em escala global têm relevância sistêmica.

A tese segura é:

```text
Sistema_financeiro_sistêmico -> transparência + supervisão + auditoria + teste_de_concentração + mitigação_de_efeito_cascata
```

Não afirmar ilegalidade sem prova específica. Exigir governança proporcional ao impacto.

## 14. Relação com RAFAELIA/Bitraf

```text
input/corpus
-> normalized chunk
-> BLAKE3/SHA3 digest
-> hashchain
-> Bitraf64 / ZIPRAF metadata
-> RAFCODE-Φ signature layer
-> GitHub/Termux reproducibility record
```

Essa cadeia preserva a fronteira correta:

- BLAKE3 fornece o primitivo mensurável de hash;
- RAFAELIA fornece orquestração, custódia, metadados, índice simbólico e contexto reprodutível;
- GitHub/Termux registram histórico operacional verificável.

## 15. Fontes para verificação pública

- BLAKE3 paper/repositório: https://github.com/BLAKE3-team/BLAKE3/blob/master/doc/blake3.pdf
- GDPR: https://eur-lex.europa.eu/eli/reg/2016/679/oj/eng
- DSA: https://eur-lex.europa.eu/eli/reg/2022/2065/oj/eng
- LGPD: https://www.planalto.gov.br/ccivil_03/_ato2015-2018/2018/lei/l13709.htm
- ECA: https://www.planalto.gov.br/ccivil_03/leis/l8069.htm
- FTC report on social media/video data practices: https://www.ftc.gov/reports/look-behind-screens-examining-data-practices-social-media-video-streaming-services
- Google Maps Platform pricing: https://developers.google.com/maps/billing-and-pricing/pricing
- BlackRock Aladdin: https://www.blackrock.com/aladdin

## 16. Retroalimentação RAFAELIA

**F_ok:** fragmentação como blindagem operacional é forte e documentável como problema de governança.  
**F_gap:** extorsão, conspiração, ilegalidade total, dolo algorítmico, blindagem intencional e claims Fiber H exigem prova interna, regulatória ou benchmark próprio.  
**F_next:** gerar artefatos BLAKE3 para documentos, capturas de metadados, protocolos, logs auditáveis e matriz Fiber H.
