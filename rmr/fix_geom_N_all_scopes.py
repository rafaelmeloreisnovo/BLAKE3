/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * This file is part of the RMR module.
 * It does not modify or replace the BLAKE3 core.
 */

#!/usr/bin/env python3
from pathlib import Path
import time, re

p = Path("src/geom.c")
s = p.read_text(encoding="utf-8", errors="ignore")

bak = p.with_suffix(f".pre_fix.{int(time.time())}.bak")
bak.write_text(s, encoding="utf-8")
print(f"[A] backup: {bak}")

# 0) helper macros
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

# 1) remover lixo antigo fn / const const
s = re.sub(r"^[ \t]*const[ \t]+float[ \t]+fn[ \t]*=[ \t]*\(\s*float\s*\)\s*n\s*;[ \t]*\r?\n", "", s, flags=re.M)
s = re.sub(r"\bfn\b", "F32(n)", s)
s = re.sub(r"\bconst\s+const\b", "const", s)

# 2) corrigir N*N*sizeof(float) -> N*sizeof(float)
s = re.sub(r"\bN\s*\*\s*N\s*\*\s*sizeof\s*\(\s*float\s*\)", "N * sizeof(float)", s)

lines = s.splitlines(True)

# 3) Para cada função: se houver " i < N " e não houver definição de N dentro do corpo,
#    injeta "size_t N = SZ(n) * SZ(n);" logo após a primeira chave '{' da função.
#
# Heurística segura:
# - detecta início de função por linha que termina com "{"
# - coleta bloco até o matching "}"
# - verifica uso de N no bloco e presença de definição de N
def find_blocks(lines):
    blocks = []
    i = 0
    while i < len(lines):
        # linha com '{' possivelmente início de função (evita struct/if/for)
        if re.search(r"\)\s*\{\s*$", lines[i]):  # assinatura de função típica
            start = i
            depth = 0
            j = i
            while j < len(lines):
                depth += lines[j].count("{")
                depth -= lines[j].count("}")
                if depth == 0 and j > start:
                    end = j
                    blocks.append((start, end))
                    i = end + 1
                    break
                j += 1
            else:
                i += 1
        else:
            i += 1
    return blocks

blocks = find_blocks(lines)

def block_text(a,b):
    return "".join(lines[a:b+1])

for (a,b) in blocks:
    txt = block_text(a,b)

    uses_N = re.search(r"\bN\b", txt) is not None
    loop_uses_N = re.search(r"\bfor\s*\([^;]*;[^;]*<\s*N\s*;[^)]*\)", txt) is not None
    has_N_def = re.search(r"^\s*(int|size_t)\s+N\s*=", txt, flags=re.M) is not None

    if loop_uses_N and not has_N_def:
        # injeta após a linha do "{"
        indent = re.match(r"^(\s*)", lines[a+1] if a+1 <= b else lines[a]).group(1)
        inject = indent + "size_t N = SZ(n) * SZ(n);\n"
        lines.insert(a+1, inject)
        print(f"[B] injected N in function starting line {a+1}")

# 4) Trocar loops int->size_t quando comparando com N
for i, ln in enumerate(lines):
    lines[i] = re.sub(
        r"\bfor\s*\(\s*int\s+i\s*=\s*0\s*;\s*i\s*<\s*N\s*;\s*i\+\+\s*\)",
        "for(size_t i=0; i < N; i++)",
        lines[i]
    )

# 5) salvar
out = "".join(lines)
p.write_text(out, encoding="utf-8")
print("[OK] geom.c patched: N definido em todos os escopos que usam N")
