#!/usr/bin/env sh
set -eu

ROOT=$(git rev-parse --show-toplevel)
OUT=${1:-/tmp/rmr-repository-source-manifest.tsv}

classify() {
  p=$1
  case "$p" in
    rmr/portable/*) echo "RMR_PORTABLE_V1|LicenseRef-RMR-Individual-Research-1.0" ;;
    rmr/*) echo "RMR_LEGACY|RMR_MODULE_PRIOR_GRANT" ;;
    src/*|c/*|b3sum/*|reference_impl/*|test_vectors/*|benches/*|media/*)
      echo "UPSTREAM_BLAKE3|UPSTREAM_LICENSES" ;;
    audit/*|audits/*|auditoria/*)
      echo "RMR_AUDIT|PRIOR_APPLICABLE_GRANT" ;;
    .github/workflows/rmr-*|DOCUMENTACAO.md|FORK_NOTES.md|MANIFESTO*.md|RELATORIO*.md|AGENTS.md)
      echo "RMR_EXTERNAL_AUTHORED|PRIOR_APPLICABLE_GRANT" ;;
    *)
      echo "MIXED_OR_UNRESOLVED|TOKEN_VAZIO_PROVENANCE" ;;
  esac
}

printf 'sha256\torigin_class\tlicense_class\tpath\n' > "$OUT"
git -C "$ROOT" ls-files | LC_ALL=C sort | while IFS= read -r p; do
  test -f "$ROOT/$p" || continue
  h=$(sha256sum "$ROOT/$p" | awk '{print $1}')
  c=$(classify "$p")
  o=${c%%|*}
  l=${c#*|}
  printf '%s\t%s\t%s\t%s\n' "$h" "$o" "$l" "$p" >> "$OUT"
done

count=$(tail -n +2 "$OUT" | wc -l | tr -d ' ')
unresolved=$(awk -F'\t' '$2=="MIXED_OR_UNRESOLVED"{n++} END{print n+0}' "$OUT")
portable=$(awk -F'\t' '$2=="RMR_PORTABLE_V1"{n++} END{print n+0}' "$OUT")
echo "RMR_SOURCE_MANIFEST_FILES=$count"
echo "RMR_SOURCE_MANIFEST_PORTABLE=$portable"
echo "RMR_SOURCE_MANIFEST_UNRESOLVED=$unresolved"
sha256sum "$OUT"
