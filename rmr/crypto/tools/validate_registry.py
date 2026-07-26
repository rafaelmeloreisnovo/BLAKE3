#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

"""Deterministic, stdlib-only validation for the RMR crypto registry."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_REGISTRY = ROOT / "registry" / "architectures.json"
DEFAULT_CLAIMS = ROOT / "claims" / "claims.jsonl"
REPO_RE = re.compile(r"^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$")
EXPECTED_IDS = {
    "openssl",
    "boringssl",
    "aws-lc",
    "mbedtls",
    "wolfssl",
    "openssh-portable",
    "libsodium",
    "blake3",
    "liboqs",
    "cryptopp",
}
RELATIONSHIP_STATES = {
    "VERIFIED_DIRECT_PARENT",
    "VERIFIED_ANCESTOR",
    "RELATED_DISTRIBUTION",
    "TOKEN_VAZIO_PARENT_VERIFICATION",
    "NOT_A_FORK",
}
LICENSE_STATES = {
    "VERIFIED_AT_COMMIT",
    "DECLARED_BY_REPOSITORY",
    "DECLARED_BY_REPOSITORY_REQUIRES_COMMIT_AUDIT",
    "TOKEN_VAZIO_NOT_REVIEWED",
    "CONFLICT_REQUIRES_HUMAN_REVIEW",
}
CLAIM_STATES = {
    "VERIFIED_PRIMARY",
    "VERIFIED_LOCAL",
    "DOCUMENTED",
    "DECLARED",
    "TOKEN_VAZIO",
    "REFUTED_IN_SCOPE",
}


class ValidationError(Exception):
    """Raised for one or more contract violations."""


def load_json(path: Path) -> Any:
    try:
        with path.open("r", encoding="utf-8") as handle:
            return json.load(handle)
    except FileNotFoundError as exc:
        raise ValidationError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValidationError(f"invalid JSON {path}:{exc.lineno}:{exc.colno}: {exc.msg}") from exc


def validate_registry(path: Path) -> tuple[list[str], str]:
    errors: list[str] = []
    raw = path.read_bytes()
    registry_sha256 = hashlib.sha256(raw).hexdigest()
    data = load_json(path)

    if not isinstance(data, dict):
        raise ValidationError("registry root must be an object")

    meta = data.get("_meta")
    families = data.get("families")
    if not isinstance(meta, dict):
        errors.append("_meta must be an object")
        meta = {}
    if meta.get("schema") != "rmr.crypto.architecture-registry.v1":
        errors.append("unexpected registry schema")
    if meta.get("claim_allowed") is not False:
        errors.append("registry claim_allowed must be false")
    if meta.get("reference_only") is not True:
        errors.append("registry must remain reference_only")
    if meta.get("third_party_code_vendored") is not False:
        errors.append("third_party_code_vendored must be false")
    if meta.get("human_legal_review_required") is not True:
        errors.append("human_legal_review_required must be true")
    if meta.get("local_integration_repository") != "rafaelmeloreisnovo/BLAKE3":
        errors.append("unexpected local integration repository")
    if meta.get("license") != "Licensed under LICENSE_RMR.":
        errors.append("registry attribution/license metadata missing")

    if not isinstance(families, list):
        errors.append("families must be an array")
        return errors, registry_sha256
    if len(families) != 10:
        errors.append(f"expected 10 families, found {len(families)}")

    ids: set[str] = set()
    repositories: set[str] = set()
    candidate_count = 0

    for index, family in enumerate(families):
        prefix = f"families[{index}]"
        if not isinstance(family, dict):
            errors.append(f"{prefix} must be an object")
            continue

        family_id = family.get("id")
        if not isinstance(family_id, str) or not family_id:
            errors.append(f"{prefix}.id missing")
        elif family_id in ids:
            errors.append(f"duplicate family id: {family_id}")
        else:
            ids.add(family_id)

        upstream = family.get("upstream_repository")
        if not isinstance(upstream, str) or not REPO_RE.fullmatch(upstream):
            errors.append(f"{prefix}.upstream_repository invalid")
        elif upstream in repositories:
            errors.append(f"duplicate repository: {upstream}")
        else:
            repositories.add(upstream)

        if family.get("vendored") is not False:
            errors.append(f"{prefix}.vendored must be false")
        if family.get("license_status") not in LICENSE_STATES:
            errors.append(f"{prefix}.license_status invalid")
        if family.get("license_status") == "VERIFIED_AT_COMMIT":
            errors.append(
                f"{prefix}: VERIFIED_AT_COMMIT requires a future evidence field and is blocked in v1"
            )
        if not isinstance(family.get("rmr_role"), str) or not family.get("rmr_role"):
            errors.append(f"{prefix}.rmr_role missing")
        if not isinstance(family.get("import_policy"), str) or not family.get("import_policy"):
            errors.append(f"{prefix}.import_policy missing")

        candidates = family.get("candidate_forks")
        if not isinstance(candidates, list):
            errors.append(f"{prefix}.candidate_forks must be an array")
            continue
        if len(candidates) != 3:
            errors.append(f"{prefix}: expected 3 candidates, found {len(candidates)}")
        candidate_count += len(candidates)

        for candidate_index, candidate in enumerate(candidates):
            candidate_prefix = f"{prefix}.candidate_forks[{candidate_index}]"
            if not isinstance(candidate, dict):
                errors.append(f"{candidate_prefix} must be an object")
                continue
            repo = candidate.get("repository")
            if not isinstance(repo, str) or not REPO_RE.fullmatch(repo):
                errors.append(f"{candidate_prefix}.repository invalid")
            elif repo in repositories:
                errors.append(f"duplicate repository: {repo}")
            else:
                repositories.add(repo)
            if not isinstance(candidate.get("archived_observed"), bool):
                errors.append(f"{candidate_prefix}.archived_observed must be boolean")
            if candidate.get("relationship_status") not in RELATIONSHIP_STATES:
                errors.append(f"{candidate_prefix}.relationship_status invalid")

    if ids != EXPECTED_IDS:
        errors.append(
            "family id set mismatch: "
            f"missing={sorted(EXPECTED_IDS - ids)} extra={sorted(ids - EXPECTED_IDS)}"
        )
    if candidate_count != 30:
        errors.append(f"expected 30 candidates, found {candidate_count}")

    return errors, registry_sha256


def validate_claims(path: Path) -> list[str]:
    errors: list[str] = []
    seen: set[str] = set()
    meta_seen = False

    try:
        lines = path.read_text(encoding="utf-8").splitlines()
    except FileNotFoundError as exc:
        raise ValidationError(f"missing file: {path}") from exc

    for line_number, line in enumerate(lines, start=1):
        if not line.strip():
            continue
        try:
            record = json.loads(line)
        except json.JSONDecodeError as exc:
            errors.append(f"claims line {line_number}: invalid JSON: {exc.msg}")
            continue
        if not isinstance(record, dict):
            errors.append(f"claims line {line_number}: record must be object")
            continue
        if record.get("record_type") == "meta":
            if line_number != 1:
                errors.append("claims metadata must be first record")
            if meta_seen:
                errors.append("duplicate claims metadata")
            meta_seen = True
            if record.get("schema") != "rmr.crypto.claims.v1":
                errors.append("unexpected claims schema")
            if record.get("claim_allowed") is not False:
                errors.append("claims metadata claim_allowed must be false")
            continue
        if record.get("record_type") != "claim":
            errors.append(f"claims line {line_number}: unsupported record_type")
            continue

        claim_id = record.get("claim_id")
        if not isinstance(claim_id, str) or not claim_id:
            errors.append(f"claims line {line_number}: claim_id missing")
        elif claim_id in seen:
            errors.append(f"duplicate claim_id: {claim_id}")
        else:
            seen.add(claim_id)

        status = record.get("status")
        if status not in CLAIM_STATES:
            errors.append(f"{claim_id}: invalid status {status!r}")
        evidence = record.get("evidence")
        if not isinstance(evidence, list):
            errors.append(f"{claim_id}: evidence must be array")
        elif status in {"VERIFIED_PRIMARY", "VERIFIED_LOCAL"} and not evidence:
            errors.append(f"{claim_id}: verified claim requires evidence")
        if not isinstance(record.get("falsifier"), str) or not record.get("falsifier"):
            errors.append(f"{claim_id}: falsifier missing")
        if status in {"TOKEN_VAZIO", "REFUTED_IN_SCOPE"} and record.get("claim_allowed") is not False:
            errors.append(f"{claim_id}: non-promoted claim must have claim_allowed=false")

    if not meta_seen:
        errors.append("claims metadata record missing")
    if not seen:
        errors.append("no claims found")
    return errors


def validate_sha256_kats() -> list[str]:
    errors: list[str] = []
    vectors = {
        b"": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        b"abc": "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
    }
    for message, expected in vectors.items():
        observed = hashlib.sha256(message).hexdigest()
        if observed != expected:
            errors.append(
                f"SHA-256 KAT failed for {message!r}: expected={expected} observed={observed}"
            )
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--registry", type=Path, default=DEFAULT_REGISTRY)
    parser.add_argument("--claims", type=Path, default=DEFAULT_CLAIMS)
    parser.add_argument("--json", action="store_true", help="emit machine-readable report")
    args = parser.parse_args()

    try:
        registry_errors, registry_sha256 = validate_registry(args.registry)
        claims_errors = validate_claims(args.claims)
        kat_errors = validate_sha256_kats()
    except (OSError, ValidationError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2

    errors = registry_errors + claims_errors + kat_errors
    report = {
        "schema": "rmr.crypto.audit-report.v1",
        "registry": str(args.registry),
        "registry_sha256": registry_sha256,
        "family_count_expected": 10,
        "candidate_count_expected": 30,
        "sha256_kat_status": "PASS" if not kat_errors else "FAIL",
        "claims_status": "PASS" if not claims_errors else "FAIL",
        "registry_status": "PASS" if not registry_errors else "FAIL",
        "error_count": len(errors),
        "errors": errors,
        "status": "PASS" if not errors else "FAIL",
    }

    if args.json:
        print(json.dumps(report, ensure_ascii=False, sort_keys=True, separators=(",", ":")))
    else:
        print(f"registry={args.registry}")
        print(f"registry_sha256={registry_sha256}")
        print("families=10")
        print("candidates=30")
        print(f"sha256_kat_status={report['sha256_kat_status']}")
        print(f"claims_status={report['claims_status']}")
        print(f"registry_status={report['registry_status']}")
        print(f"status={report['status']}")
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)

    return 0 if not errors else 1


if __name__ == "__main__":
    raise SystemExit(main())
