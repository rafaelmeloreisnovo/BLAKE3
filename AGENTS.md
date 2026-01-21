# AGENTS.md — Instruções Operacionais do Repositório

## 1. Papel deste arquivo
Este arquivo define as regras de leitura, revisão e modificação do repositório
para humanos e ferramentas de IA (ex.: Codex).

Qualquer análise ou alteração DEVE começar por aqui.

---

## 2. Identidade do Projeto

- Projeto: BLAKE3
- Origem: Fork do BLAKE3 oficial
- Status do ZIP: Conteúdo baixado (distribuição), não um projeto novo

O BLAKE3 upstream é tratado como NÚCLEO ABSOLUTO.

---

## 3. Núcleo Absoluto (UPSTREAM)

Considerados núcleo e SEMPRE tratados como upstream:

- c/blake3*.c
- c/blake3*.h
- c/*.S
- c/*.asm
- Qualquer arquivo `.rs` do BLAKE3
- Test vectors e especificação
- README.md e LICENSE do BLAKE3 oficial

### Regras do Núcleo
- ❌ Não alterar lógica criptográfica
- ❌ Não alterar constantes, IVs, flags ou rounds
- ❌ Não introduzir decisões de arquitetura
- ❌ Não incluir headers externos
- ✅ Apenas ajustes de build/organização quando estritamente necessários

---

## 4. Camadas Externas / Autorais

Qualquer código fora do núcleo é considerado CAMADA EXTERNA, por exemplo:

- rmr/
- tools/
- scripts/
- wrappers
- benchmarks
- CLIs
- automação
- auditoria

### Regras da Camada Externa
- Pode USAR o BLAKE3 apenas como biblioteca
- Nunca pode modificar nem invadir o núcleo
- Deve estar claramente documentada como externa

---

## 5. Linguagens — Classificação

- C / H / ASM / RS:
  - Núcleo criptográfico (se estiverem nos paths do BLAKE3)
- PY / SH:
  - Infraestrutura, build, testes ou automação
  - Devem ficar fora do núcleo

---

## 6. Ordem Obrigatória de Trabalho

1. Ler AGENTS.md
2. Ler README.md
3. Ler LICENSE
4. Auditar arquivos (classificar núcleo vs externo)
5. Só então propor modificações

---

## 7. Critério de Aceitação

Uma alteração é aceita se:
- O BLAKE3 continua funcionalmente idêntico ao upstream
- Diferenças se limitam a organização, build ou documentação
- Camadas externas estão isoladas e explícitas

---

## 8. Proibição Geral

- ❌ Renomear o projeto BLAKE3
- ❌ Criar novo hash
- ❌ “Otimizar” criptografia
- ❌ Misturar sistema com motor criptográfico
