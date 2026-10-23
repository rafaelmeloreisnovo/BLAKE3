#!/usr/bin/env sh
# Copyright (c) 2024-2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.
#
# Exact-source, two-pass, six-ELF replay. Never edits an input file.
# Run only from a trusted, exact-SHA checkout. Not a physical-device benchmark.

set -eu
export LC_ALL=C
export TZ=UTC
umask 077

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "$ROOT"

: "${RMR_EXPECTED_SHA:?pin the exact commit SHA}"
actual=$(git rev-parse HEAD)
if [ "$actual" != "$RMR_EXPECTED_SHA" ]; then
  echo "RMR_BIT_IDENTITY=FAIL_EXACT_HEAD"
  exit 1
fi

for tool in git clang ld.lld readelf nm size stat sha256sum cmp sort xargs; do
  if ! command -v "$tool" >/dev/null 2>&1; then
    echo "RMR_BIT_IDENTITY=BLOCKED_MISSING_TOOL:$tool"
    exit 1
  fi
done

export SOURCE_DATE_EPOCH
SOURCE_DATE_EPOCH=$(git show -s --format=%ct "$RMR_EXPECTED_SHA")
WORK=$(mktemp -d "${TMPDIR:-/tmp}/rmr-bit-identity.XXXXXXXX")
trap 'rm -rf "$WORK"' EXIT HUP INT TERM

source_manifest() {
  git ls-files -z -- rmr | xargs -0 -r sha256sum | LC_ALL=C sort
}

source_manifest > "$WORK/source_before.sha256"
git ls-files -z -- rmr | xargs -0 -r stat -c "%n\t%s" | LC_ALL=C sort > "$WORK/source_size_before.tsv"
if [ ! -s "$WORK/source_before.sha256" ]; then
  echo "RMR_BIT_IDENTITY=FAIL_EMPTY_MANIFEST"
  exit 1
fi

{
  echo "schema=rmr.binary-identity/v1"
  echo "source_head=$RMR_EXPECTED_SHA"
  echo "source_date_epoch=$SOURCE_DATE_EPOCH"
  echo "target_set=x86_64,aarch64,armv7"
  echo "source_manifest_sha256=$(sha256sum "$WORK/source_before.sha256" | awk '{print $1}')"
  echo "source_file_count=$(wc -l < "$WORK/source_before.sha256" | tr -d ' ')"
  echo "source_size_manifest_sha256=$(sha256sum "$WORK/source_size_before.tsv" | awk '{print $1}')"
  echo "build_script_sha256=$(sha256sum rmr/portable/build/build_cross_matrix.sh | awk '{print $1}')"
  echo "host_script_sha256=$(sha256sum rmr/portable/build/build_host_selftest.sh | awk '{print $1}')"
  echo "compiler=$(clang --version | head -n 1)"
  echo "linker=$(ld.lld --version | head -n 1)"
  echo "readelf=$(readelf --version | head -n 1)"
  echo "warning_policy=-Wshadow,-Werror (RMR strict source)"
  echo "void_policy=source_audit_not_optimization_claim"
  echo "binary_metadata=readelf-nm-size"
} > "$WORK/receipt.txt"

echo "RMR_BIT_IDENTITY_START_HEAD=$RMR_EXPECTED_SHA"
# Semantic and constructed-negative-collision gates run before bit replay.
sh rmr/portable/audit/audit_source_shape.sh
sh rmr/portable/build/build_host_selftest.sh
echo "source_shape_and_host_kats=PASS" >> "$WORK/receipt.txt"

# The existing authoritative build script generates three pure-core and
# three provider-backed ELF artifacts. OUT is outside the Git checkout.
OUT="$WORK/build_a" sh rmr/portable/build/build_cross_matrix.sh
OUT="$WORK/build_b" sh rmr/portable/build/build_cross_matrix.sh

for name in core-x86_64 core-aarch64 core-armv7 blake3-x86_64 blake3-aarch64 blake3-armv7; do
  a="$WORK/build_a/$name"
  b="$WORK/build_b/$name"
  if [ ! -s "$a" ] || [ ! -s "$b" ]; then
    echo "RMR_BIT_IDENTITY=FAIL_MISSING_ELF:$name"
    exit 1
  fi
  if ! cmp -s "$a" "$b"; then
    echo "RMR_BIT_IDENTITY=FAIL_BINARY_DIFF:$name"
    echo "build_a_sha256=$(sha256sum "$a" | awk '{print $1}')"
    echo "build_b_sha256=$(sha256sum "$b" | awk '{print $1}')"
    exit 1
  fi

  # Read-only inspection of final ELF and link metadata. These fingerprints
  # are observational; they cannot substitute for a physical runtime receipt.
  readelf -W -h -l -S -s -r "$a" > "$WORK/$name.readelf.txt"
  nm -an "$a" > "$WORK/$name.nm.txt"
  size -A "$a" > "$WORK/$name.size.txt"

  {
    echo "$name.byte_identical=true"
    echo "$name.sha256=$(sha256sum "$a" | awk '{print $1}')"
    echo "$name.bytes=$(stat -c %s "$a")"
    echo "$name.readelf_report_sha256=$(sha256sum "$WORK/$name.readelf.txt" | awk '{print $1}')"
    echo "$name.nm_report_sha256=$(sha256sum "$WORK/$name.nm.txt" | awk '{print $1}')"
    echo "$name.size_report_sha256=$(sha256sum "$WORK/$name.size.txt" | awk '{print $1}')"
  } >> "$WORK/receipt.txt"
  echo "RMR_TWO_PASS_BINARY_EQUAL=$name"
done

source_manifest > "$WORK/source_after.sha256"
git ls-files -z -- rmr | xargs -0 -r stat -c "%n\t%s" | LC_ALL=C sort > "$WORK/source_size_after.tsv"
if ! cmp -s "$WORK/source_size_before.tsv" "$WORK/source_size_after.tsv"; then
  echo "RMR_BIT_IDENTITY=FAIL_SOURCE_SIZES_CHANGED"
  exit 1
fi
if ! cmp -s "$WORK/source_before.sha256" "$WORK/source_after.sha256"; then
  echo "RMR_BIT_IDENTITY=FAIL_SOURCE_BYTES_CHANGED"
  exit 1
fi
if ! git diff --quiet -- rmr; then
  echo "RMR_BIT_IDENTITY=FAIL_DIRTY_SOURCE"
  exit 1
fi

{
  echo "source_bytes_unchanged=true"
  echo "cross_arch_build_artifacts=6"
  echo "same_target_two_pass_byte_identity=PASS_SCOPED"
  echo "cross_target_binary_equivalence=NOT_CLAIMED"
  echo "physical_device_execution=NOT_RUN"
  echo "independent_reproduction=TOKEN_VAZIO"
  echo "performance_superiority=NOT_CLAIMED"
  echo "release_allowed=false"
  echo "claim_allowed=false"
} >> "$WORK/receipt.txt"

echo "RMR_BINARY_IDENTITY_RECEIPT_BEGIN"
cat "$WORK/receipt.txt"
echo "RMR_BINARY_IDENTITY_RECEIPT_SHA256=$(sha256sum "$WORK/receipt.txt" | awk '{print $1}')"
echo "RMR_BINARY_IDENTITY_RECEIPT_END"
if [ -n "${RMR_RECEIPT_OUTPUT:-}" ]; then
  cp "$WORK/receipt.txt" "$RMR_RECEIPT_OUTPUT"
fi
