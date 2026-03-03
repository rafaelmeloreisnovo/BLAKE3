<!--
Copyright (c) 2025 Rafael
License: RMR Module License (see LICENSE_RMR)
-->

# Relatório de Auditoria Total — Fork BLAKE3 vs Upstream

## Metadados de geração (reprodutibilidade)

- Data/hora (UTC): **2026-03-03T10:45:00Z**
- Commit-base local analisado: **`1dd4251`**
- Commit de referência upstream: **`fb1411e`** (`upstream/master`)
- Comandos-base usados:
  - `git fetch upstream master --quiet`
  - `git diff --name-status upstream/master > audit/diff_name_status.txt`
  - `git diff --stat upstream/master > audit/diff_stat.txt`
  - `git ls-files`
  - `find . -maxdepth 2 -type d | sort`

---

## 0) Inventário do repositório

### 0.1 Árvore de diretórios (profundidade 2)

```text
.
./.cargo
./.git
./.git/branches
./.git/hooks
./.git/info
./.git/logs
./.git/objects
./.git/refs
./.github
./.github/workflows
./audit
./b3sum
./b3sum/src
./b3sum/tests
./benches
./c
./c/blake3_c_rust_bindings
./c/cmake
./c/dependencies
./media
./reference_impl
./rmr
./rmr/asm
./rmr/benchmark_framework
./rmr/detect
./rmr/include
./rmr/rust
./src
./test_vectors
./test_vectors/src
./tools
./tools/compiler_version
./tools/instruction_set_support
```

### 0.2 Contagem de arquivos por tipo (rastreado por `git ls-files`)

| Extensão | Contagem |
| --- | ---: |
| `.rs` | 34 |
| `.c` | 29 |
| `.h` | 17 |
| `.S` | 17 |
| `.asm` | 4 |
| `.md` | 21 |
| `.sh` | 19 |
| `.toml` | 8 |
| `.yml` | 2 |
| `.yaml` | 0 |
| `.json` | 2 |
| `.py` | 7 |
| `.patch` | 7 |
| `.txt` | 5 |
| `.o` | 0 |
| `.bak` | 0 |
| `NOEXT` | 17 |
| **TOTAL** | **201** |

### 0.3 Artefatos antigos (`rmr/*.o`, `rmr/*.bak`)

- **Status atual no repositório rastreado:** não existem arquivos `rmr/*.o` nem `rmr/*.bak`.
- **Tratamento no relatório:** qualquer menção prévia a esses artefatos deve ser lida como **histórico de auditorias anteriores**, não como estado atual.

---

## 1) Diferenças vs upstream oficial

### 1.1 Sumário (fonte única)

Fonte única para ambos os relatórios: `audit/diff_name_status.txt` gerado por `git diff --name-status upstream/master`.

- Total de arquivos divergentes vs upstream: **130**
- Por status:
  - **A (Added): 93**
  - **M (Modified): 37**
- Classificação por camada:
  - **CORE (upstream): 40**
  - **EXTERNAL (RMR/auditoria/docs externos): 90**

### 1.2 Estatística de linhas

Fonte: `audit/diff_stat.txt` gerado por `git diff --stat upstream/master`.

- **130 files changed, 12998 insertions(+), 271 deletions(-)**

### 1.3 Referências de evidência (atualizadas)

- `audit/diff_name_status.txt`
- `audit/diff_stat.txt`
- `audit/diff_c.patch`
- `audit/diff_src.patch`
- `audit/diff_rmr.patch`
- `audit/diff_tools.patch`

---

## 2) Nota de consistência

Este `RELATORIO.md` e `RELATORIO_AUDITORIA.md` foram alinhados para usar:

1. mesma referência upstream (`upstream/master` @ `fb1411e`),
2. mesma base local (`1dd4251`),
3. mesmas fontes de métrica (`audit/diff_name_status.txt` e `audit/diff_stat.txt`),
4. mesma terminologia (`CORE` vs `EXTERNAL`).
