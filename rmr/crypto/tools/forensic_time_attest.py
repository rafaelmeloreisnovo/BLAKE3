#!/usr/bin/env python3
# Copyright (c) 2024–2026 Rafael Melo Reis
# Licensed under LICENSE_RMR.

"""Create/validate RMR forensic time records without promoting local time to trusted time."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
DEFAULT_PROFILE = ROOT / "rmr/crypto/registry/forensic_time_profile.jsonl"

UTC_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$")
LOCAL_RE = re.compile(r"^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}([+-]\d{2}:\d{2})$")
HEX_RE = re.compile(r"^[0-9a-fA-F]+$")
COMMIT_RE = re.compile(r"^[0-9a-fA-F]{40}$")
CLOCK_SOURCES = {"SYSTEM_CLOCK", "NTP", "GPS", "TSA", "MANUAL", "TOKEN_VAZIO"}
SYNC_STATES = {"SYNCHRONIZED", "UNVERIFIED", "TOKEN_VAZIO"}
DIGEST_LENGTHS = {"SHA-256": 64, "BLAKE3-256": 64, "GIT-OID-SHA1": 40}

DOMAIN_INSTANT = b"RMR-FORENSIC-INSTANT-V1\0"
DOMAIN_LOCAL = b"RMR-FORENSIC-LOCAL-ATTESTATION-V1\0"
DOMAIN_SIGNATURE = b"RMR-FORENSIC-TIME-SIGNATURE-V1\0"


class ValidationError(RuntimeError):
    pass


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValidationError(message)


def canonical_json(value: Any) -> bytes:
    return json.dumps(
        value,
        ensure_ascii=False,
        sort_keys=True,
        separators=(",", ":"),
    ).encode("utf-8")


def load_profile(path: Path = DEFAULT_PROFILE) -> dict[str, Any]:
    rows = [
        json.loads(line)
        for line in path.read_text(encoding="utf-8").splitlines()
        if line.strip()
    ]
    require(len(rows) == 2, "forensic time profile must contain meta + profile rows")
    meta, profile = rows
    require(meta.get("record_type") == "meta", "first profile row must be meta")
    require(meta.get("schema") == "rmr.forensic_time_profile.v1", "unexpected profile schema")
    require(meta.get("claim_allowed") is False, "profile meta claim_allowed must remain false")
    require(meta.get("human_review_required") is True, "human review must remain required")
    require(profile.get("record_type") == "profile", "second row must be profile")
    require(profile.get("profile_id") == "RMR-FORENSIC-TIME-V1", "unexpected profile id")
    require(profile.get("claim_allowed") is False, "profile claim_allowed must remain false")
    require(profile["canonical_time"]["scale"] == "UTC", "canonical scale must be UTC")
    require(profile["canonical_time"]["daylight_saving_affects_canonical_time"] is False,
            "DST must not alter canonical UTC")
    require(profile["freestanding_custody16"]["core_reads_clock"] is False,
            "Custody16 core must remain clock-free")
    return profile


def parse_utc(value: str) -> datetime:
    require(isinstance(value, str) and UTC_RE.fullmatch(value) is not None,
            "created_at_utc must be RFC3339 whole-second UTC ending in Z")
    try:
        return datetime.strptime(value, "%Y-%m-%dT%H:%M:%SZ").replace(tzinfo=timezone.utc)
    except ValueError as exc:
        raise ValidationError(f"invalid created_at_utc: {exc}") from exc


def parse_local(value: str) -> datetime:
    require(isinstance(value, str) and LOCAL_RE.fullmatch(value) is not None,
            "local_time_rfc3339 must include an explicit numeric UTC offset")
    try:
        parsed = datetime.fromisoformat(value)
    except ValueError as exc:
        raise ValidationError(f"invalid local_time_rfc3339: {exc}") from exc
    require(parsed.tzinfo is not None and parsed.utcoffset() is not None,
            "local timestamp must be offset-aware")
    return parsed


def format_utc(value: datetime) -> str:
    return value.astimezone(timezone.utc).replace(microsecond=0).strftime("%Y-%m-%dT%H:%M:%SZ")


def format_local(value: datetime) -> str:
    value = value.replace(microsecond=0)
    require(value.tzinfo is not None and value.utcoffset() is not None,
            "local datetime must be offset-aware")
    return value.isoformat(timespec="seconds")


def offset_string(value: datetime) -> str:
    offset = value.utcoffset()
    require(offset is not None, "local offset missing")
    seconds = int(offset.total_seconds())
    sign = "+" if seconds >= 0 else "-"
    seconds = abs(seconds)
    hours, rem = divmod(seconds, 3600)
    minutes = rem // 60
    require(hours <= 23 and minutes <= 59 and rem % 60 == 0,
            "V1 supports minute-granularity offsets only")
    return f"{sign}{hours:02d}:{minutes:02d}"


def validate_digest(evidence_digest: dict[str, Any]) -> dict[str, str]:
    require(isinstance(evidence_digest, dict), "evidence_digest must be an object")
    algorithm = evidence_digest.get("algorithm")
    value = evidence_digest.get("value")
    require(algorithm in DIGEST_LENGTHS, f"unsupported evidence digest algorithm: {algorithm}")
    require(isinstance(value, str) and HEX_RE.fullmatch(value) is not None,
            "evidence digest must be hexadecimal")
    require(len(value) == DIGEST_LENGTHS[algorithm], "evidence digest length mismatch")
    return {"algorithm": algorithm, "value": value.lower()}


def validate_record(record: dict[str, Any]) -> dict[str, Any]:
    require(isinstance(record, dict), "record must be an object")
    require(record.get("schema") == "rmr.forensic_time.record.v1", "unexpected record schema")
    require(record.get("claim_allowed") is False, "claim_allowed must remain false")

    evidence_id = record.get("evidence_id")
    require(isinstance(evidence_id, str) and evidence_id.strip(), "missing evidence_id")
    digest = validate_digest(record.get("evidence_digest"))

    utc_dt = parse_utc(record.get("created_at_utc"))
    local_dt = parse_local(record.get("local_time_rfc3339"))

    epoch = record.get("created_at_unix_s")
    require(isinstance(epoch, int) and not isinstance(epoch, bool), "created_at_unix_s must be integer")
    require(int(utc_dt.timestamp()) == epoch, "UTC timestamp and Unix epoch identify different seconds")
    require(local_dt.astimezone(timezone.utc) == utc_dt,
            "local timestamp and canonical UTC identify different instants")

    declared_offset = record.get("utc_offset")
    require(isinstance(declared_offset, str), "utc_offset missing")
    require(declared_offset == offset_string(local_dt),
            "declared utc_offset differs from local timestamp offset")

    timezone_id = record.get("timezone_id")
    require(isinstance(timezone_id, str) and timezone_id.strip(), "timezone_id missing")

    clock_source = record.get("clock_source")
    sync_status = record.get("clock_sync_status")
    require(clock_source in CLOCK_SOURCES, "unsupported clock_source")
    require(sync_status in SYNC_STATES, "unsupported clock_sync_status")

    uncertainty = record.get("clock_uncertainty_ms")
    if uncertainty is not None:
        require(isinstance(uncertainty, int) and not isinstance(uncertainty, bool) and uncertainty >= 0,
                "clock_uncertainty_ms must be a nonnegative integer")

    source_commit = record.get("source_commit", "TOKEN_VAZIO")
    require(source_commit == "TOKEN_VAZIO" or COMMIT_RE.fullmatch(source_commit) is not None,
            "source_commit must be a full 40-hex Git SHA or TOKEN_VAZIO")

    operator = record.get("operator", "TOKEN_VAZIO")
    require(isinstance(operator, str) and operator.strip(), "operator missing")

    if sync_status == "SYNCHRONIZED":
        require(clock_source not in {"MANUAL", "TOKEN_VAZIO"},
                "SYNCHRONIZED requires a synchronization-capable declared clock source")

    normalized = dict(record)
    normalized["evidence_id"] = evidence_id.strip()
    normalized["evidence_digest"] = digest
    normalized["created_at_utc"] = format_utc(utc_dt)
    normalized["created_at_unix_s"] = epoch
    normalized["local_time_rfc3339"] = format_local(local_dt)
    normalized["utc_offset"] = offset_string(local_dt)
    normalized["timezone_id"] = timezone_id.strip()
    normalized["source_commit"] = source_commit.lower() if source_commit != "TOKEN_VAZIO" else source_commit
    normalized["operator"] = operator.strip()
    normalized["claim_allowed"] = False
    return normalized


def derive(record: dict[str, Any]) -> dict[str, str]:
    record = validate_record(record)

    instant_object = {
        "schema": "rmr.forensic_time.instant.v1",
        "evidence_id": record["evidence_id"],
        "evidence_digest": record["evidence_digest"],
        "created_at_utc": record["created_at_utc"],
        "created_at_unix_s": record["created_at_unix_s"],
    }
    instant_digest = hashlib.sha256(DOMAIN_INSTANT + canonical_json(instant_object)).hexdigest()

    local_object = {
        "schema": "rmr.forensic_time.local_attestation.v1",
        "instant_digest_sha256": instant_digest,
        "local_time_rfc3339": record["local_time_rfc3339"],
        "utc_offset": record["utc_offset"],
        "timezone_id": record["timezone_id"],
        "clock_source": record["clock_source"],
        "clock_sync_status": record["clock_sync_status"],
        "clock_uncertainty_ms": record.get("clock_uncertainty_ms"),
        "source_commit": record["source_commit"],
        "operator": record["operator"],
    }
    local_digest = hashlib.sha256(DOMAIN_LOCAL + canonical_json(local_object)).hexdigest()
    signature_preimage = hashlib.sha256(
        DOMAIN_SIGNATURE + bytes.fromhex(local_digest)
    ).hexdigest()

    return {
        "instant_digest_sha256": instant_digest,
        "local_attestation_digest_sha256": local_digest,
        "signature_preimage_sha256": signature_preimage,
        "forensic_state": "STRUCTURALLY_VALID_LOCAL_ATTESTATION",
        "digital_signature_state": "TOKEN_VAZIO",
        "trusted_external_timestamp_state": "TOKEN_VAZIO",
    }


def capture_now_record(
    *,
    evidence_id: str,
    evidence_algorithm: str,
    evidence_digest: str,
    timezone_id: str,
    clock_source: str,
    clock_sync_status: str,
    clock_uncertainty_ms: int | None,
    source_commit: str,
    operator: str,
) -> dict[str, Any]:
    local_now = datetime.now().astimezone().replace(microsecond=0)
    utc_now = local_now.astimezone(timezone.utc)
    record: dict[str, Any] = {
        "schema": "rmr.forensic_time.record.v1",
        "evidence_id": evidence_id,
        "evidence_digest": {"algorithm": evidence_algorithm, "value": evidence_digest},
        "source_commit": source_commit,
        "created_at_utc": format_utc(utc_now),
        "created_at_unix_s": int(utc_now.timestamp()),
        "local_time_rfc3339": format_local(local_now),
        "utc_offset": offset_string(local_now),
        "timezone_id": timezone_id,
        "clock_source": clock_source,
        "clock_sync_status": clock_sync_status,
        "operator": operator,
        "claim_allowed": False,
    }
    if clock_uncertainty_ms is not None:
        record["clock_uncertainty_ms"] = clock_uncertainty_ms
    return validate_record(record)


def build_explicit_record(args: argparse.Namespace) -> dict[str, Any]:
    missing = [
        name for name, value in (
            ("--utc", args.utc),
            ("--epoch", args.epoch),
            ("--local", args.local),
        ) if value is None
    ]
    require(not missing, f"explicit mode missing: {', '.join(missing)}")
    record: dict[str, Any] = {
        "schema": "rmr.forensic_time.record.v1",
        "evidence_id": args.evidence_id,
        "evidence_digest": {"algorithm": args.evidence_algorithm, "value": args.evidence_digest},
        "source_commit": args.source_commit,
        "created_at_utc": args.utc,
        "created_at_unix_s": args.epoch,
        "local_time_rfc3339": args.local,
        "utc_offset": LOCAL_RE.fullmatch(args.local).group(1) if LOCAL_RE.fullmatch(args.local) else "",
        "timezone_id": args.timezone_id,
        "clock_source": args.clock_source,
        "clock_sync_status": args.clock_sync_status,
        "operator": args.operator,
        "claim_allowed": False,
    }
    if args.clock_uncertainty_ms is not None:
        record["clock_uncertainty_ms"] = args.clock_uncertainty_ms
    return validate_record(record)


def emit(record: dict[str, Any], output: Path | None) -> None:
    envelope = {
        "record": validate_record(record),
        "derived": derive(record),
    }
    rendered = json.dumps(envelope, ensure_ascii=False, sort_keys=True, indent=2) + "\n"
    if output is None:
        sys.stdout.write(rendered)
    else:
        output.write_text(rendered, encoding="utf-8")


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--capture-now", action="store_true",
                      help="capture local system time once, then derive UTC/epoch")
    mode.add_argument("--record", type=Path, help="validate an existing record JSON")

    parser.add_argument("--utc", help="canonical UTC whole-second RFC3339 timestamp ending in Z")
    parser.add_argument("--epoch", type=int, help="Unix epoch seconds for explicit mode")
    parser.add_argument("--local", help="local RFC3339 timestamp with numeric offset")
    parser.add_argument("--evidence-id")
    parser.add_argument("--evidence-algorithm", choices=sorted(DIGEST_LENGTHS), default="SHA-256")
    parser.add_argument("--evidence-digest")
    parser.add_argument("--source-commit", default="TOKEN_VAZIO")
    parser.add_argument("--timezone-id", default="TOKEN_VAZIO")
    parser.add_argument("--clock-source", choices=sorted(CLOCK_SOURCES), default="SYSTEM_CLOCK")
    parser.add_argument("--clock-sync-status", choices=sorted(SYNC_STATES), default="TOKEN_VAZIO")
    parser.add_argument("--clock-uncertainty-ms", type=int)
    parser.add_argument("--operator", default="TOKEN_VAZIO")
    parser.add_argument("--profile", type=Path, default=DEFAULT_PROFILE)
    parser.add_argument("--output", type=Path)
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        load_profile(args.profile)
        if args.record is not None:
            record = json.loads(args.record.read_text(encoding="utf-8"))
        else:
            require(args.evidence_id is not None and args.evidence_id.strip(), "--evidence-id is required")
            require(args.evidence_digest is not None and args.evidence_digest.strip(),
                    "--evidence-digest is required")
            if args.capture_now:
                record = capture_now_record(
                    evidence_id=args.evidence_id,
                    evidence_algorithm=args.evidence_algorithm,
                    evidence_digest=args.evidence_digest,
                    timezone_id=args.timezone_id,
                    clock_source=args.clock_source,
                    clock_sync_status=args.clock_sync_status,
                    clock_uncertainty_ms=args.clock_uncertainty_ms,
                    source_commit=args.source_commit,
                    operator=args.operator,
                )
            else:
                record = build_explicit_record(args)
        emit(validate_record(record), args.output)
    except (OSError, json.JSONDecodeError, ValidationError) as exc:
        print(f"RMR_FORENSIC_TIME=FAIL: {exc}", file=sys.stderr)
        return 1
    print("RMR_FORENSIC_TIME=PASS", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
