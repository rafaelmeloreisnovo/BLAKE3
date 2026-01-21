#!/data/data/com.termux/files/usr/bin/sh
set -eu

# ===== config =====
BIN="${1:-./pai}"
OUTDIR="${2:-out_bench}"
N="${N:-7}"            # repeticoes
SIZE="${SIZE:-1024}"
NU="${NU:-256}"
NV="${NV:-128}"

mkdir -p "$OUTDIR"
OUT="$OUTDIR/bench.tsv"

echo "op\trep\tms\tok" > "$OUT"

ms_now() {
  # Termux: date +%s%3N funciona na maioria; fallback para seconds*1000
  if date +%s%3N >/dev/null 2>&1; then
    date +%s%3N
  else
    python - <<'PY'
import time
print(int(time.time()*1000))
PY
  fi
}

run_op() {
  op="$1"
  shift
  rep="$1"
  shift

  t0=$(ms_now)
  ok=1
  if "$BIN" "$op" "$@" >/dev/null 2>&1; then
    ok=1
  else
    ok=0
  fi
  t1=$(ms_now)
  dt=$((t1 - t0))
  echo "${op}\t${rep}\t${dt}\t${ok}" >> "$OUT"
}

# ===== prepara dados =====
rm -rf "$OUTDIR/tmp"
mkdir -p "$OUTDIR/tmp"

# arquivo teste
printf "abc" > "$OUTDIR/tmp/test.txt"

# ===== bench =====
i=1
while [ "$i" -le "$N" ]; do
  run_op "hash"   "$i" --file "$OUTDIR/tmp/test.txt"

  rm -rf "$OUTDIR/tmp/scan_in" "$OUTDIR/tmp/scan_out"
  mkdir -p "$OUTDIR/tmp/scan_in"
  cp "$OUTDIR/tmp/test.txt" "$OUTDIR/tmp/scan_in/a.txt"
  run_op "scan"   "$i" --base "$OUTDIR/tmp/scan_in" --out "$OUTDIR/tmp/scan_out"

  rm -rf "$OUTDIR/tmp/geom"
  run_op "geom"   "$i" --out "$OUTDIR/tmp/geom" --size "$SIZE" --cycle42 --sin30 --sqrt2 --sqrt3 --fibo --shapes --mandel

  rm -rf "$OUTDIR/tmp/toroid"
  run_op "toroid" "$i" --tex "$OUTDIR/tmp/geom/geom.pgm" --out "$OUTDIR/tmp/toroid" --nu "$NU" --nv "$NV"

  rm -rf "$OUTDIR/tmp/toroid_scan" "$OUTDIR/tmp/toroid_signed"
  run_op "scan"   "$i" --base "$OUTDIR/tmp/toroid" --out "$OUTDIR/tmp/toroid_scan"
  run_op "sign"   "$i" --base "$OUTDIR/tmp/toroid" --scan "$OUTDIR/tmp/toroid_scan" --out "$OUTDIR/tmp/toroid_signed"

  i=$((i+1))
done

echo "[OK] bench salvo em: $OUT"
echo "Dica: ordenar por op e ver medias:"
echo "  awk 'NR>1{a[\$1]+=\$3; c[\$1]++} END{for(k in a) printf(\"%s\\t%.2f ms\\t(n=%d)\\n\", k, a[k]/c[k], c[k])}' $OUT | sort"
