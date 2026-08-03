#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
umask 077

REPO_URL="${1:-https://github.com/BLAKE3-team/BLAKE3.git}"
OUT_ROOT="${2:-$HOME/storage/shared/RAFAELIA_AUDIT/BLAKE3}"
STAMP="${3:-$(date -u +%Y%m%dT%H%M%SZ)}"
MIRROR_ROOT="$OUT_ROOT/mirror"
MIRROR="$MIRROR_ROOT/BLAKE3-upstream.git"
SNAP="$OUT_ROOT/snapshots/$STAMP"

mkdir -p "$MIRROR_ROOT" "$SNAP"/{refs,logs,objects,fsck,metadata,receipts}

redact_url() {
  sed -E 's#(https?://)[^/@]+@#\1REDACTED@#g'
}

if [ ! -d "$MIRROR/objects" ]; then
  git clone --mirror "$REPO_URL" "$MIRROR"
else
  git -C "$MIRROR" remote update --prune
fi

{
  printf 'snapshot_id=%s\n' "$STAMP"
  printf 'captured_at_utc=%s\n' "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  printf 'captured_at_local=%s\n' "$(date +%Y-%m-%dT%H:%M:%S%z)"
  printf 'uname=%s\n' "$(uname -a)"
  printf 'git=%s\n' "$(git --version)"
  printf 'shell=%s\n' "${BASH_VERSION:-unknown}"
  printf 'repo_url='
  git -C "$MIRROR" config --get remote.origin.url | redact_url
} > "$SNAP/metadata/ENVIRONMENT.txt"

git -C "$MIRROR" show-ref --head | sort > "$SNAP/refs/show-ref.txt"
git -C "$MIRROR" for-each-ref \
  --sort=refname \
  --format='%(refname)%09%(objectname)%09%(objecttype)%09%(authordate:iso-strict)%09%(committerdate:iso-strict)%09%(taggerdate:iso-strict)%09%(subject)' \
  > "$SNAP/refs/for-each-ref.tsv"

git -C "$MIRROR" log --all --date=iso-strict --pretty=raw \
  > "$SNAP/logs/log-all.pretty-raw.txt"
git -C "$MIRROR" log --all --date=iso-strict \
  --pretty=format:'%H%x09%P%x09%T%x09%an%x09%ae%x09%aI%x09%cn%x09%ce%x09%cI%x09%G?%x09%s' \
  > "$SNAP/logs/commit-index.tsv"

git -C "$MIRROR" rev-list --all --objects \
  > "$SNAP/objects/rev-list-all-objects.txt"
git -C "$MIRROR" cat-file \
  --batch-check='%(objectname)%09%(objecttype)%09%(objectsize)%09%(objectsize:disk)' \
  --batch-all-objects > "$SNAP/objects/object-index.tsv"

git -C "$MIRROR" fsck --full --strict --no-reflogs --unreachable \
  > "$SNAP/fsck/fsck.txt" 2>&1 || true
git -C "$MIRROR" replace -l > "$SNAP/refs/replace-refs.txt" 2>&1 || true
git -C "$MIRROR" notes list > "$SNAP/refs/notes.txt" 2>&1 || true
git -C "$MIRROR" tag -n999 > "$SNAP/refs/tags-with-subjects.txt" 2>&1 || true

(
  cd "$SNAP"
  find . -type f \
    ! -name 'SHA256SUMS.txt' \
    ! -name 'BLAKE3SUMS.txt' \
    -print0 | sort -z | xargs -0 sha256sum > receipts/SHA256SUMS.txt
  if command -v b3sum >/dev/null 2>&1; then
    find . -type f \
      ! -name 'SHA256SUMS.txt' \
      ! -name 'BLAKE3SUMS.txt' \
      -print0 | sort -z | xargs -0 b3sum > receipts/BLAKE3SUMS.txt
  fi
)

printf '%s\n' "$SNAP"
printf 'capture_complete=true\n'
