#!/usr/bin/env sh
set -eu

ROOT=$(git rev-parse --show-toplevel)
OUT=${1:-/tmp/rmr-repository-source-manifest.tsv}
COMPARE="$ROOT/rmr/portable/registry/UPSTREAM_BLOB_COMPARE_V1.tsv"

relation_for() {
  p=$1
  awk -F '\t' -v p="$p" 'NR>1 && $1==p {print $4; exit}' "$COMPARE"
}

classify() {
  p=$1
  case "$p" in
    rmr/portable/*) echo "RMR_PORTABLE_V1|LicenseRef-RMR-Individual-Research-1.0"; return ;;
    rmr/*) echo "RMR_LEGACY|RMR_MODULE_PRIOR_GRANT"; return ;;
    src/*|c/*|b3sum/*|reference_impl/*|test_vectors/*|benches/*|media/*)
      echo "UPSTREAM_BLAKE3|UPSTREAM_LICENSES"; return ;;
    audit/*|audits/*|auditoria/*)
      echo "RMR_AUDIT|PRIOR_APPLICABLE_GRANT"; return ;;
    .github/workflows/rmr-*|DOCUMENTACAO.md|FORK_NOTES.md|MANIFESTO*.md|RELATORIO*.md|AGENTS.md)
      echo "RMR_EXTERNAL_AUTHORED|PRIOR_APPLICABLE_GRANT"; return ;;
  esac

  relation=$(relation_for "$p")
  case "$relation" in
    EXACT)
      echo "UPSTREAM_BLAKE3_EXACT|UPSTREAM_LICENSES"
      return
      ;;
    DIVERGENT)
      echo "UPSTREAM_PATH_DIVERGENT_BLOB|UPSTREAM_OR_FORK_MODIFIED_REVIEW_REQUIRED"
      return
      ;;
    ABSENT)
      case "$p" in
        Captura\ de\ tela*.png)
          echo "FORK_MEDIA_ADDITION|TOKEN_VAZIO_MEDIA_RIGHTS"
          ;;
        THIRD_PARTY_NOTICES_RAFAELIA.md|docs/RAFAELIA_*|docs/rafaelia/*|rafaelia/*)
          echo "FORK_RAFAELIA_MATERIAL|TOKEN_VAZIO_FILE_LICENSE_REVIEW"
          ;;
        tools/*)
          echo "FORK_TOOLING_ADDITION|TOKEN_VAZIO_FILE_LICENSE_REVIEW"
          ;;
        *)
          echo "FORK_ADDITION|TOKEN_VAZIO_FILE_LICENSE_REVIEW"
          ;;
      esac
      return
      ;;
  esac

  echo "MIXED_OR_UNRESOLVED|TOKEN_VAZIO_PROVENANCE"
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
up_exact=$(awk -F'\t' '$2=="UPSTREAM_BLAKE3_EXACT"{n++} END{print n+0}' "$OUT")
up_divergent=$(awk -F'\t' '$2=="UPSTREAM_PATH_DIVERGENT_BLOB"{n++} END{print n+0}' "$OUT")
fork_added=$(awk -F'\t' '$2 ~ /^FORK_/ {n++} END{print n+0}' "$OUT")
license_token=$(awk -F'\t' '$3 ~ /TOKEN_VAZIO/ {n++} END{print n+0}' "$OUT")

echo "RMR_SOURCE_MANIFEST_FILES=$count"
echo "RMR_SOURCE_MANIFEST_PORTABLE=$portable"
echo "RMR_SOURCE_MANIFEST_UPSTREAM_EXACT=$up_exact"
echo "RMR_SOURCE_MANIFEST_UPSTREAM_DIVERGENT=$up_divergent"
echo "RMR_SOURCE_MANIFEST_FORK_ADDITIONS=$fork_added"
echo "RMR_SOURCE_MANIFEST_UNRESOLVED_ORIGIN=$unresolved"
echo "RMR_SOURCE_MANIFEST_LICENSE_TOKEN_VAZIO=$license_token"
sha256sum "$OUT"
