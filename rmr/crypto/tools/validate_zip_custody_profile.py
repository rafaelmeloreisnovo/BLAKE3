#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

"""Fail-closed, stdlib-only validation for the RMR ZIP custody profile."""

from __future__ import annotations

import argparse
import binascii
import hashlib
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
DEFAULT_PROFILE = ROOT / "rmr/crypto/registry/zip_custody_profile.json"
DEFAULT_CLAIMS = ROOT / "rmr/crypto/claims/zip_bitstack_claims.jsonl"
SOURCE = ROOT / "rmr/src/rmr_visual_prototype.c"
CONTRACT = ROOT / "rmr/VISUAL_PROTOTYPE_CONTRACT.md"

EXPECTED_LAYER_IDS = [f"L{i}" for i in range(8)]
REQUIRED_FIELDS = {
    "capsule_format",
    "capsule_version",
    "domain_word_utf8_sha256",
    "payload_size",
    "payload_crc32c",
    "payload_sha256",
    "archive_method",
    "archive_crc32",
    "archive_sha256",
    "source_commit",
    "parent_evidence_digest",
    "created_at_utc",
    "toolchain",
    "claim_allowed",
}
FORBIDDEN_PROMOTIONS = {
    "CRC_MATCH_IMPLIES_AUTHENTICITY",
    "BIT_STACKING_IMPLIES_CONFIDENTIALITY",
    "ZIP_CONTAINER_IMPLIES_ENCRYPTION",
    "PUBLIC_WORD_IMPLIES_SECRET_KEY",
    "HASHCHAIN_IMPLIES_DISTRIBUTED_CONSENSUS",
    "LOCAL_FIXTURE_IMPLIES_UNIVERSAL_SECURITY",
}
SOURCE_MARKERS = (
    "uint32_t RmR_Visual_ClassId",
    "return RmR_Stability_CRC32C(label, n);",
    "uint32_t RmR_VisualPrototype_Serialize",
    "const uint32_t capsule_crc = RmR_Stability_CRC32C",
    "out->class_id != RmR_Visual_ClassId(out->label)",
)


class ValidationError(RuntimeError):
    pass


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValidationError(message)


def canonical_json(value: Any) -> bytes:
    return json.dumps(value, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")


def crc32c(data: bytes) -> int:
    crc = 0xFFFFFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            mask = -(crc & 1) & 0xFFFFFFFF
            crc = ((crc >> 1) ^ (0x82F63B78 & mask)) & 0xFFFFFFFF
    return crc ^ 0xFFFFFFFF


def load_json(path: Path) -> dict[str, Any]:
    value = json.loads(path.read_text(encoding="utf-8"))
    require(isinstance(value, dict), f"{path}: root must be an object")
    return value


def load_claims(path: Path) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for line_number, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        if not raw.strip():
            continue
        value = json.loads(raw)
        require(isinstance(value, dict), f"{path}:{line_number}: row must be an object")
        rows.append(value)
    return rows


def validate_known_vectors() -> None:
    vector = b"123456789"
    require(crc32c(vector) == 0xE3069283, "CRC32C known-answer test failed")
    require((binascii.crc32(vector) & 0xFFFFFFFF) == 0xCBF43926, "ZIP CRC-32 known-answer test failed")
    require(
        hashlib.sha256(vector).hexdigest()
        == "15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225",
        "SHA-256 known-answer test failed",
    )


def validate_profile(profile: dict[str, Any]) -> None:
    meta = profile.get("_meta")
    require(isinstance(meta, dict), "missing _meta")
    require(meta.get("schema") == "rmr.zip_bitstack_custody.v1", "unexpected schema")
    require(meta.get("claim_allowed") is False, "meta claim_allowed must remain false")
    require(meta.get("human_review_required") is True, "human review must be required")
    require(profile.get("claim_allowed") is False, "profile claim_allowed must remain false")

    layers = profile.get("layers")
    require(isinstance(layers, list), "layers must be a list")
    require([item.get("id") for item in layers] == EXPECTED_LAYER_IDS, "layers must be exactly L0..L7 in order")
    require(len({item.get("name") for item in layers}) == len(layers), "layer names must be unique")

    by_id = {item["id"]: item for item in layers}
    require(by_id["L1"].get("security_state") == "STRUCTURAL_ENCODING_NOT_ENCRYPTION", "L1 boundary changed")
    require(by_id["L3"].get("security_state") == "ERROR_DETECTION_NOT_MAC", "L3 boundary changed")
    require(by_id["L7"].get("implemented_state") == "TOKEN_VAZIO_EXTERNAL_ANCHOR", "L7 must remain TOKEN_VAZIO")

    fixture = profile.get("known_fixture")
    require(isinstance(fixture, dict), "known_fixture missing")
    require(fixture.get("payload_size_bytes") == 140, "unexpected fixture payload size")
    require(fixture.get("zip_size_bytes") == 272, "unexpected fixture ZIP size")
    require(fixture.get("zip_method") == 0, "ZIP method must be STORE/0")
    require(fixture.get("zip_crc32") == "2d8bd598", "unexpected fixture ZIP CRC-32")
    require(
        fixture.get("payload_sha256") == "3438ca62d78667862f86fe809463ce328fc025e763fdd7a8ec1693344726dfed",
        "unexpected fixture payload SHA-256",
    )
    require(fixture.get("roundtrip_state") == "VERIFIED_BYTE_FOR_BYTE", "roundtrip evidence changed")

    word = profile.get("word_binding")
    require(isinstance(word, dict), "word_binding missing")
    require(word.get("current_mechanism") == "CRC32C(label) -> class_id", "word binding mechanism changed")
    require(word.get("word_is_secret") is False, "public word must not be promoted to secret")
    require(word.get("key_derivation_role") == "FORBIDDEN_WITH_RAW_CRC", "raw CRC key derivation must stay forbidden")

    manifest_fields = set(profile.get("required_manifest_fields") or [])
    require(manifest_fields == REQUIRED_FIELDS, "required manifest field set changed")
    promotions = set(profile.get("prohibited_promotions") or [])
    require(promotions == FORBIDDEN_PROMOTIONS, "prohibited promotion set changed")


def validate_claims(rows: list[dict[str, Any]]) -> None:
    require(len(rows) >= 2, "claims ledger must include meta and claims")
    meta = rows[0]
    require(meta.get("record_type") == "meta", "first claims row must be meta")
    require(meta.get("claim_allowed") is False, "claims meta must remain false")
    claims = rows[1:]
    ids = [row.get("claim_id") for row in claims]
    require(len(ids) == len(set(ids)), "duplicate claim_id")
    require(ids == [f"RMR-ZIP-{i:03d}" for i in range(1, 9)], "claims must be RMR-ZIP-001..008")
    for row in claims:
        require(row.get("claim_allowed") is False, f"{row.get('claim_id')}: claim_allowed must remain false")
        require(bool(row.get("statement")), f"{row.get('claim_id')}: missing statement")
        require(bool(row.get("falsifier")), f"{row.get('claim_id')}: missing falsifier")
    last = claims[-1]
    require(last.get("status") == "TOKEN_VAZIO_NOT_IMPLEMENTED", "keyed mode claim must remain TOKEN_VAZIO")
    require(last.get("evidence") == [], "unimplemented keyed mode cannot have positive evidence")


def validate_primary_source() -> None:
    source = SOURCE.read_text(encoding="utf-8")
    for marker in SOURCE_MARKERS:
        require(marker in source, f"primary source marker missing: {marker}")
    contract = CONTRACT.read_text(encoding="utf-8")
    for marker in (
        "ZIP STORE payload",
        "CRC32C and the capsule checksum prove deterministic identity, not semantic truth",
        "ZIP CRC-32 was `2d8bd598`",
        "payload SHA-256 was `3438ca62d78667862f86fe809463ce328fc025e763fdd7a8ec1693344726dfed`",
    ):
        require(marker in contract, f"contract evidence marker missing: {marker}")


def run(profile_path: Path, claims_path: Path) -> dict[str, Any]:
    validate_known_vectors()
    profile = load_json(profile_path)
    claims = load_claims(claims_path)
    validate_profile(profile)
    validate_claims(claims)
    validate_primary_source()
    profile_sha256 = hashlib.sha256(canonical_json(profile)).hexdigest()
    claims_sha256 = hashlib.sha256(claims_path.read_bytes()).hexdigest()
    return {
        "schema": profile["_meta"]["schema"],
        "profile_id": profile["profile_id"],
        "layers": len(profile["layers"]),
        "claims": len(claims) - 1,
        "crc32c_kat": "PASS",
        "zip_crc32_kat": "PASS",
        "sha256_kat": "PASS",
        "primary_source_markers": "PASS",
        "profile_sha256": profile_sha256,
        "claims_sha256": claims_sha256,
        "claim_allowed": False,
        "status": "PASS",
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--profile", type=Path, default=DEFAULT_PROFILE)
    parser.add_argument("--claims", type=Path, default=DEFAULT_CLAIMS)
    parser.add_argument("--json", action="store_true", help="emit compact JSON")
    args = parser.parse_args(argv)
    try:
        result = run(args.profile, args.claims)
    except (OSError, json.JSONDecodeError, ValidationError) as exc:
        print(f"RMR_ZIP_CUSTODY=FAIL: {exc}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps(result, sort_keys=True, separators=(",", ":")))
    else:
        for key, value in result.items():
            print(f"{key}={str(value).lower() if isinstance(value, bool) else value}")
        print("RMR_ZIP_CUSTODY=PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
