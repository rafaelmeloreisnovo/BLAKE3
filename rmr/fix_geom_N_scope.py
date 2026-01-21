#!/usr/bin/env python3
from pathlib import Path
import time
import re

p = Path("src/geom.c")
s = p.read_text(encoding="utf-8", errors="ignore")

# 0) backup
bak = p.with_suffix(f".pre_fix.{int(time.time())}.bak")
bak.write_text(s, encoding="utf-8")
print(f"[A] backup: {bak}")

# 1) garantir helpers F32/SZ (uma vez)
if "PAI_CAST_HELPERS" not in s:
    macro = (
        "\n#ifndef PAI_CAST_HELPERS\n"
        "#define PAI_CAST_HELPERS\n"
        "#include <stddef.h>\n"
        "#define F32(x) ((float)(x))\n"
        "#define SZ(x)  ((size_t)(x))\n"
        "#endif\n"
    )
    incs = list(re.finditer(r"^#include[^\n]*$", s, flags=re.M))
    if incs:
        pos = incs[-1].end()
        s = s[:pos] + macro + s[pos:]
    else:
        s = macro + s

# 2) remover qualquer declaração "const float fn = (float)n;" (se sobrou lixo)
s = re.sub(r"^[ \t]*const[ \t]+float[ \t]+fn[ \t]*=[ \t]*\(\s*float\s*\)\s*n\s*;[ \t]*\r?\n", "", s, flags=re.M)

# 3) substituir token isolado fn -> F32(n) (se ainda existir em algum lugar)
s = re.sub(r"\bfn\b", "F32(n)", s)

# 4) limpar "const const"
s = re.sub(r"\bconst\s+const\b", "const", s)

# 5) Corrigir o bug: N*N*sizeof(float) -> N*sizeof(float) (se existir)
s = re.sub(r"\bN\s*\*\s*N\s*\*\s*sizeof\s*\(\s*float\s*\)", "N * sizeof(float)", s)

# 6) Garantir que EXISTE um "size_t N = SZ(n)*SZ(n);" NO ESCOPO CERTO
# Estratégia: localizar a primeira alocação do bloco ov/mb/gf e injetar N logo antes.
lines = s.splitlines(True)

# achar linha do primeiro "float *ov = (float*)xmalloc(" (ou "xmalloc(" perto de ov)
idx_ov = None
for i, ln in enumerate(lines):
    if "float *ov" in ln and "xmalloc" in ln:
        idx_ov = i
        break

if idx_ov is None:
    raise SystemExit("[ERRO] nao achei a linha de alocacao do ov (float *ov = ... xmalloc).")

# remover quaisquer definicoes antigas de N muito acima/abaixo que possam estar fora do lugar
# (somente se forem exatamente "int N = n*n;" ou "size_t N = ...;" soltas)
new_lines = []
for ln in lines:
    if re.search(r"^\s*(int|size_t)\s+N\s*=\s*.*;\s*$", ln):
        # vamos remover e reinserir no lugar certo
        continue
    new_lines.append(ln)
lines = new_lines

# recalcular idx_ov porque a lista pode ter mudado
idx_ov = None
for i, ln in enumerate(lines):
    if "float *ov" in ln and "xmalloc" in ln:
        idx_ov = i
        break
if idx_ov is None:
    raise SystemExit("[ERRO] apos limpeza, nao achei ov/xmalloc.")

# injetar definicao de N imediatamente antes do ov
indent = re.match(r"^(\s*)", lines[idx_ov]).group(1)
inject = indent + "size_t N = SZ(n) * SZ(n);\n"
lines.insert(idx_ov, inject)

# 7) Ajustar qualquer loop "for(int i=0;i<N;i++)" -> size_t
for i, ln in enumerate(lines):
    lines[i] = re.sub(r"\bfor\s*\(\s*int\s+i\s*=\s*0\s*;\s*i\s*<\s*N\s*;\s*i\+\+\s*\)",
                      "for(size_t i=0; i < N; i++)", lines[i])

# 8) garantir includes de <stddef.h> já via macro, e salvar
out = "".join(lines)
p.write_text(out, encoding="utf-8")
print("[B] patched: N reinserido no escopo certo + loop size_t + limpeza fn/constconst")
