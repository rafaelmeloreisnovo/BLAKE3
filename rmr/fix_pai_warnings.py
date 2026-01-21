#!/usr/bin/env python3
import re
from pathlib import Path

def load(p: Path) -> str:
    return p.read_text(encoding="utf-8", errors="ignore")

def save(p: Path, s: str):
    p.write_text(s, encoding="utf-8")

def ensure_once(s: str, needle: str, insert_after_re: str, insert_text: str) -> str:
    if needle in s:
        return s
    m = re.search(insert_after_re, s, flags=re.M)
    if not m:
        # if can't find anchor, prepend
        return insert_text + "\n" + s
    idx = m.end()
    return s[:idx] + "\n" + insert_text + s[idx:]

def sub_idem(s: str, pattern: str, repl: str, flags=0) -> str:
    # only replace if pattern exists and repl not already present nearby
    if re.search(pattern, s, flags):
        s = re.sub(pattern, repl, s, flags=flags)
    return s

def fix_hash_sha256(path: Path):
    s = load(path)

    # Ensure <stdint.h> (should exist, but safe)
    s = ensure_once(s, "#include <stdint.h>", r"^#include\s+<stdio\.h>.*$", "#include <stdint.h>")

    # w[i] message schedule cast fix
    # Try to match a typical line constructing uint32_t from 4 bytes
    s = sub_idem(
        s,
        r"w\[(?:i|t)\]\s*=\s*\(block\[[^\]]+\]\s*<<\s*24\)\s*\|\s*\(block\[[^\]]+\]\s*<<\s*16\)\s*\|\s*\(block\[[^\]]+\]\s*<<\s*8\)\s*\|\s*\(block\[[^\]]+\]\)\s*;",
        "w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) | ((uint32_t)block[i*4+2] << 8) | ((uint32_t)block[i*4+3]);",
        flags=re.M,
    )
    # Another common form: w[i] = (block[i*4]<<24)|...
    s = sub_idem(
        s,
        r"w\[i\]\s*=\s*\(block\[i\*4\]\s*<<\s*24\)\s*\|\s*\(block\[i\*4\+1\]\s*<<\s*16\)\s*\|\s*\(block\[i\*4\+2\]\s*<<\s*8\)\s*\|\s*\(block\[i\*4\+3\]\)\s*;",
        "w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16) | ((uint32_t)block[i*4+2] << 8) | ((uint32_t)block[i*4+3]);",
        flags=re.M,
    )

    # Output bytes explicit truncation to uint8_t
    # Replace 4 lines pattern if present
    s = sub_idem(
        s,
        r"out\[(?:i)\*4\+0\]\s*=\s*ctx->h\[(?:i)\]\s*>>\s*24\s*;\s*"
        r"out\[(?:i)\*4\+1\]\s*=\s*ctx->h\[(?:i)\]\s*>>\s*16\s*;\s*"
        r"out\[(?:i)\*4\+2\]\s*=\s*ctx->h\[(?:i)\]\s*>>\s*8\s*;\s*"
        r"out\[(?:i)\*4\+3\]\s*=\s*ctx->h\[(?:i)\]\s*;\s*",
        "out[i*4+0] = (uint8_t)(ctx->h[i] >> 24);\n        out[i*4+1] = (uint8_t)(ctx->h[i] >> 16);\n        out[i*4+2] = (uint8_t)(ctx->h[i] >>  8);\n        out[i*4+3] = (uint8_t)(ctx->h[i] >>  0);\n",
        flags=re.S,
    )

    save(path, s)

def fix_toroid(path: Path):
    s = load(path)

    # Ensure includes
    s = ensure_once(s, "#include <math.h>", r"^#include\s+\".*?\"\s*$", "#include <math.h>")

    # atof -> float casts
    s = sub_idem(s, r"o\.R\s*=\s*atof\(", "o.R = (float)atof(")
    s = sub_idem(s, r"o\.r\s*=\s*atof\(", "o.r = (float)atof(")

    # Safer size_t for read buffer if pattern exists
    # Transform:
    # unsigned char *buf = xmalloc((*w)*(*h));
    # fread(buf,1,(*w)*(*h),f);
    if "unsigned char *buf = xmalloc((*w)*(*h));" in s and "const size_t tw =" not in s:
        s = s.replace(
            "unsigned char *buf = xmalloc((*w)*(*h));\n    fread(buf,1,(*w)*(*h),f);",
            "const size_t tw = (size_t)(*w);\n    const size_t th = (size_t)(*h);\n    unsigned char *buf = xmalloc(tw * th);\n    (void)fread(buf, 1, tw * th, f);",
        )

    # fwrite(tex,1,tw*th,ft) -> size_t
    s = sub_idem(s, r"fwrite\(\s*tex\s*,\s*1\s*,\s*tw\*th\s*,\s*ft\s*\)\s*;", "fwrite(tex, 1, (size_t)tw * (size_t)th, ft);")

    # 2*M_PI float: introduce two_pi and cast divisions
    if re.search(r"\* 2\*M_PI", s) and "two_pi" not in s:
        s = re.sub(
            r"(float\s+v\s*=\s*\(float\)j/o\.nv\s*\*\s*2\*M_PI\s*;)",
            "const float two_pi = (float)(2.0 * M_PI);\n        float v = ((float)j/(float)o.nv) * two_pi;",
            s,
        )
        s = re.sub(
            r"float\s+u\s*=\s*\(float\)i/o\.nu\s*\*\s*2\*M_PI\s*;",
            "float u = ((float)i/(float)o.nu) * two_pi;",
            s,
        )

    save(path, s)

def fix_geom(path: Path):
    s = load(path)

    # Add helper macros once (keeps patch simple + avoids messing with code structure)
    macro_block = (
        "#ifndef PAI_CAST_HELPERS\n"
        "#define PAI_CAST_HELPERS\n"
        "#define F32(x) ((float)(x))\n"
        "#define SZ(x)  ((size_t)(x))\n"
        "#endif\n"
    )
    # Insert after last include
    if "PAI_CAST_HELPERS" not in s:
        # find last include line
        incs = list(re.finditer(r"^#include[^\n]*$", s, flags=re.M))
        if incs:
            last = incs[-1].end()
            s = s[:last] + "\n" + macro_block + s[last:]
        else:
            s = macro_block + s

    # Replace common int->float risky patterns
    s = s.replace("(n-1)/2.0f", "(F32(n)-1.0f)*0.5f")
    s = s.replace("(n/2.0f)", "(F32(n)*0.5f)")
    s = s.replace("0.93f*(n/2.0f)", "0.93f*(F32(n)*0.5f)")
    s = s.replace("0.85f*(n/2.0f)", "0.85f*(F32(n)*0.5f)")
    s = s.replace("(n/2.0f)", "(F32(n)*0.5f)")

    # (x - cx)/s etc
    s = re.sub(r"\(\s*x\s*-\s*cx\s*\)\s*/\s*s", "(F32(x) - cx) / s", s)
    s = re.sub(r"\(\s*y\s*-\s*cy\s*\)\s*/\s*s", "(F32(y) - cy) / s", s)

    # ((float)y/(n-1) - 0.5f) already casts y, but ensure denom float
    s = s.replace("((float)y/(n-1)", "((float)y/(F32(n)-1.0f)")
    s = s.replace("((float)x/(n-1)", "((float)x/(F32(n)-1.0f)")

    # size_t for allocations/memset: (size_t)n*n -> SZ(n)*SZ(n)
    s = re.sub(r"\(size_t\)\s*n\s*\*\s*n", "SZ(n) * SZ(n)", s)
    s = re.sub(r"\(size_t\)\s*n\s*\*\s*n\s*\*\s*sizeof\(float\)", "SZ(n) * SZ(n) * sizeof(float)", s)
    s = re.sub(r"\(size_t\)\s*n\s*\*\s*n\s*\*\s*sizeof\(\s*float\s*\)", "SZ(n) * SZ(n) * sizeof(float)", s)

    # also n*n when already inside size_t: (size_t)n*n*sizeof(float) variants
    s = re.sub(r"\(\s*SZ\(n\)\s*\)\s*\*\s*\(\s*SZ\(n\)\s*\)", "SZ(n) * SZ(n)", s)

    save(path, s)

def main():
    base = Path(".")
    targets = [
        ("src/hash_sha256.c", fix_hash_sha256),
        ("src/toroid.c", fix_toroid),
        ("src/geom.c", fix_geom),
    ]
    for rel, fn in targets:
        p = base / rel
        if p.exists():
            # backup once
            bak = p.with_suffix(p.suffix + ".bak")
            if not bak.exists():
                bak.write_text(load(p), encoding="utf-8", errors="ignore")
            fn(p)
            print(f"[OK] patched {rel}")
        else:
            print(f"[SKIP] missing {rel}")

if __name__ == "__main__":
    main()
