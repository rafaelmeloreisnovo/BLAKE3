"""
PIE-Chain MVP (REST + Python)
- Signed event ledger (Ed25519)
- Process audit without private content
- Formal exception triggers (request -> approvals -> reveal)
Run:
  pip install fastapi uvicorn cryptography pydantic
  uvicorn pie_chain_api:app --reload --port 8010
"""

from __future__ import annotations

import base64
import hashlib
import json
import os
import sqlite3
import time
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Literal

from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field

from cryptography.hazmat.primitives.asymmetric.ed25519 import (
    Ed25519PrivateKey,
    Ed25519PublicKey,
)
from cryptography.hazmat.primitives import serialization
from cryptography.fernet import Fernet, InvalidToken


DB_PATH = os.environ.get("PIE_DB", "pie_chain.db")
FERNET_KEY_PATH = os.environ.get("PIE_FERNET_KEY_FILE", "pie_fernet.key")

app = FastAPI(title="PIE-Chain MVP", version="0.1.0")


# -----------------------------
# Crypto helpers
# -----------------------------
def sha256_hex(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def b64e(b: bytes) -> str:
    return base64.urlsafe_b64encode(b).decode("utf-8")


def b64d(s: str) -> bytes:
    return base64.urlsafe_b64decode(s.encode("utf-8"))


def ensure_fernet() -> Fernet:
    if os.path.exists(FERNET_KEY_PATH):
        key = open(FERNET_KEY_PATH, "rb").read()
    else:
        key = Fernet.generate_key()
        open(FERNET_KEY_PATH, "wb").write(key)
    return Fernet(key)


FERNET = ensure_fernet()


def load_pubkey(pubkey_b64: str) -> Ed25519PublicKey:
    raw = b64d(pubkey_b64)
    return Ed25519PublicKey.from_public_bytes(raw)


def verify_ed25519(pubkey_b64: str, message_bytes: bytes, signature_b64: str) -> bool:
    try:
        pub = load_pubkey(pubkey_b64)
        sig = b64d(signature_b64)
        pub.verify(sig, message_bytes)
        return True
    except Exception:
        return False


# -----------------------------
# DB
# -----------------------------
def db() -> sqlite3.Connection:
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA journal_mode=WAL;")
    conn.execute("PRAGMA foreign_keys=ON;")
    return conn


def init_db() -> None:
    conn = db()
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS peers (
          peer_id TEXT PRIMARY KEY,
          pubkey_b64 TEXT NOT NULL,
          created_at INTEGER NOT NULL
        );
        """
    )
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS events (
          event_hash TEXT PRIMARY KEY,
          prev_hash TEXT,
          peer_id TEXT NOT NULL,
          pubkey_b64 TEXT NOT NULL,
          signature_b64 TEXT NOT NULL,

          event_type TEXT NOT NULL, -- MESSAGE | ACK | SUMMARY | EXC_REQUEST | EXC_APPROVAL | REVEAL
          scope TEXT NOT NULL,      -- private | shared | public
          timestamp INTEGER NOT NULL,

          content_hash TEXT NOT NULL,   -- sha256 of content bytes (or external payload)
          content_ref TEXT,             -- optional: uri/offset
          encrypted_blob_b64 TEXT,      -- optional encrypted content
          meta_json TEXT NOT NULL,

          FOREIGN KEY(peer_id) REFERENCES peers(peer_id)
        );
        """
    )
    conn.execute(
        """
        CREATE TABLE IF NOT EXISTS exc_cases (
          case_id TEXT PRIMARY KEY,
          requested_by TEXT NOT NULL,
          target_event_hash TEXT NOT NULL,
          reason TEXT NOT NULL,
          created_at INTEGER NOT NULL,
          status TEXT NOT NULL, -- open | approved | rejected
          quorum_n INTEGER NOT NULL,
          approvals_json TEXT NOT NULL, -- list of peer_ids who approved
          FOREIGN KEY(requested_by) REFERENCES peers(peer_id)
        );
        """
    )
    conn.commit()
    conn.close()


init_db()


# -----------------------------
# Models
# -----------------------------
Scope = Literal["private", "shared", "public"]
EventType = Literal["MESSAGE", "ACK", "SUMMARY", "EXC_REQUEST", "EXC_APPROVAL", "REVEAL"]


class PeerRegister(BaseModel):
    peer_id: str = Field(..., min_length=2, max_length=128)
    pubkey_b64: str = Field(..., description="Base64url Ed25519 public key bytes")


class EventCreate(BaseModel):
    peer_id: str
    pubkey_b64: str
    signature_b64: str

    event_type: EventType
    scope: Scope
    prev_hash: Optional[str] = None

    # content
    content: Optional[str] = Field(None, description="Optional plaintext content to encrypt & store")
    content_ref: Optional[str] = Field(None, description="Optional reference uri/offset into your vault")
    content_hash: Optional[str] = Field(None, description="If you already have a hash (blake3/sha3), supply it here")

    meta: Dict[str, Any] = Field(default_factory=dict)


class EventOut(BaseModel):
    event_hash: str
    prev_hash: Optional[str]
    peer_id: str
    event_type: EventType
    scope: Scope
    timestamp: int
    content_hash: str
    content_ref: Optional[str]
    meta: Dict[str, Any]


class AuditChainOut(BaseModel):
    ok: bool
    head: Optional[str]
    count: int
    issues: List[str]


class ExceptionRequest(BaseModel):
    case_id: str
    requested_by: str
    target_event_hash: str
    reason: str
    quorum_n: int = Field(2, ge=1, le=50)  # default small quorum for MVP


class ExceptionApprove(BaseModel):
    case_id: str
    approver_peer_id: str


class RevealRequest(BaseModel):
    case_id: str
    requester_peer_id: str


# -----------------------------
# Canonical signing payload
# -----------------------------
def canonical_event_payload(d: Dict[str, Any]) -> bytes:
    """
    Canonical bytes to verify signature.
    IMPORTANT: must not include signature itself.
    """
    canon = {
        "peer_id": d["peer_id"],
        "pubkey_b64": d["pubkey_b64"],
        "event_type": d["event_type"],
        "scope": d["scope"],
        "prev_hash": d.get("prev_hash"),
        "timestamp": d["timestamp"],
        "content_hash": d["content_hash"],
        "content_ref": d.get("content_ref"),
        "meta": d.get("meta", {}),
    }
    return json.dumps(canon, sort_keys=True, separators=(",", ":")).encode("utf-8")


# -----------------------------
# Routes
# -----------------------------
@app.post("/peers/register")
def register_peer(p: PeerRegister):
    # sanity check pubkey base64
    try:
        _ = load_pubkey(p.pubkey_b64)
    except Exception:
        raise HTTPException(400, "Invalid pubkey_b64 (must be base64url Ed25519 public key bytes).")

    conn = db()
    conn.execute(
        "INSERT OR REPLACE INTO peers(peer_id,pubkey_b64,created_at) VALUES (?,?,?)",
        (p.peer_id, p.pubkey_b64, int(time.time())),
    )
    conn.commit()
    conn.close()
    return {"ok": True, "peer_id": p.peer_id}


@app.post("/events", response_model=EventOut)
def create_event(e: EventCreate):
    ts = int(time.time())

    # content hashing
    encrypted_blob_b64 = None
    if e.content is not None:
        raw = e.content.encode("utf-8")
        # store encrypted for optional reveal later
        encrypted = FERNET.encrypt(raw)
        encrypted_blob_b64 = b64e(encrypted)
        computed_hash = sha256_hex(raw)
    else:
        computed_hash = None

    # decide content_hash
    content_hash = e.content_hash or computed_hash
    if not content_hash:
        raise HTTPException(
            400,
            "Provide either content (plaintext) or content_hash (external hash).",
        )

    # verify peer exists & pubkey matches registry (strong custody)
    conn = db()
    row = conn.execute("SELECT pubkey_b64 FROM peers WHERE peer_id=?", (e.peer_id,)).fetchone()
    if not row:
        conn.close()
        raise HTTPException(400, "Unknown peer_id. Register peer first.")
    reg_pub = row[0]
    if reg_pub != e.pubkey_b64:
        conn.close()
        raise HTTPException(400, "pubkey_b64 mismatch vs registered peer pubkey.")

    # verify signature
    payload = {
        "peer_id": e.peer_id,
        "pubkey_b64": e.pubkey_b64,
        "event_type": e.event_type,
        "scope": e.scope,
        "prev_hash": e.prev_hash,
        "timestamp": ts,
        "content_hash": content_hash,
        "content_ref": e.content_ref,
        "meta": e.meta,
    }
    msg = canonical_event_payload(payload)
    if not verify_ed25519(e.pubkey_b64, msg, e.signature_b64):
        conn.close()
        raise HTTPException(400, "Invalid signature for canonical event payload.")

    # compute event_hash (ledger id)
    event_hash = sha256_hex(msg + b"|" + b64d(e.signature_b64))

    # insert event
    conn.execute(
        """
        INSERT INTO events(
            event_hash, prev_hash, peer_id, pubkey_b64, signature_b64,
            event_type, scope, timestamp,
            content_hash, content_ref, encrypted_blob_b64, meta_json
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)
        """,
        (
            event_hash,
            e.prev_hash,
            e.peer_id,
            e.pubkey_b64,
            e.signature_b64,
            e.event_type,
            e.scope,
            ts,
            content_hash,
            e.content_ref,
            encrypted_blob_b64,
            json.dumps(e.meta, separators=(",", ":")),
        ),
    )
    conn.commit()
    conn.close()

    return EventOut(
        event_hash=event_hash,
        prev_hash=e.prev_hash,
        peer_id=e.peer_id,
        event_type=e.event_type,
        scope=e.scope,
        timestamp=ts,
        content_hash=content_hash,
        content_ref=e.content_ref,
        meta=e.meta,
    )


@app.get("/events/{event_hash}", response_model=EventOut)
def get_event(event_hash: str):
    conn = db()
    row = conn.execute(
        """
        SELECT event_hash, prev_hash, peer_id, event_type, scope, timestamp, content_hash, content_ref, meta_json
        FROM events WHERE event_hash=?
        """,
        (event_hash,),
    ).fetchone()
    conn.close()
    if not row:
        raise HTTPException(404, "Event not found.")
    return EventOut(
        event_hash=row[0],
        prev_hash=row[1],
        peer_id=row[2],
        event_type=row[3],
        scope=row[4],
        timestamp=row[5],
        content_hash=row[6],
        content_ref=row[7],
        meta=json.loads(row[8]),
    )


@app.get("/audit/chain", response_model=AuditChainOut)
def audit_chain():
    """
    Process audit:
    - verifies chain linkage existence (prev hashes)
    - verifies signatures for each event
    - DOES NOT reveal plaintext content
    """
    conn = db()
    rows = conn.execute(
        """
        SELECT event_hash, prev_hash, peer_id, pubkey_b64, signature_b64,
               event_type, scope, timestamp, content_hash, content_ref, meta_json
        FROM events
        ORDER BY timestamp ASC
        """
    ).fetchall()

    issues: List[str] = []
    event_set = {r[0] for r in rows}
    head = rows[-1][0] if rows else None

    for r in rows:
        (eh, prev, peer_id, pubkey_b64, sig_b64, et, scope, ts, ch, cref, meta_json) = r

        # prev existence
        if prev and prev not in event_set:
            issues.append(f"Missing prev_hash link: event={eh} prev={prev}")

        # signature verification
        payload = {
            "peer_id": peer_id,
            "pubkey_b64": pubkey_b64,
            "event_type": et,
            "scope": scope,
            "prev_hash": prev,
            "timestamp": ts,
            "content_hash": ch,
            "content_ref": cref,
            "meta": json.loads(meta_json),
        }
        msg = canonical_event_payload(payload)
        if not verify_ed25519(pubkey_b64, msg, sig_b64):
            issues.append(f"Bad signature: event={eh} peer={peer_id}")

        # event_hash consistency
        expected = sha256_hex(msg + b"|" + b64d(sig_b64))
        if expected != eh:
            issues.append(f"Hash mismatch: event={eh} expected={expected}")

    conn.close()
    return AuditChainOut(ok=(len(issues) == 0), head=head, count=len(rows), issues=issues)


@app.post("/exceptions/request")
def exception_request(req: ExceptionRequest):
    conn = db()

    # validate peers/events exist
    p = conn.execute("SELECT 1 FROM peers WHERE peer_id=?", (req.requested_by,)).fetchone()
    if not p:
        conn.close()
        raise HTTPException(400, "Unknown requested_by peer.")
    ev = conn.execute("SELECT 1 FROM events WHERE event_hash=?", (req.target_event_hash,)).fetchone()
    if not ev:
        conn.close()
        raise HTTPException(400, "Unknown target_event_hash.")

    conn.execute(
        """
        INSERT OR REPLACE INTO exc_cases(
          case_id, requested_by, target_event_hash, reason, created_at, status, quorum_n, approvals_json
        ) VALUES (?,?,?,?,?,?,?,?)
        """,
        (
            req.case_id,
            req.requested_by,
            req.target_event_hash,
            req.reason,
            int(time.time()),
            "open",
            req.quorum_n,
            json.dumps([], separators=(",", ":")),
        ),
    )
    conn.commit()
    conn.close()
    return {"ok": True, "case_id": req.case_id, "status": "open", "quorum_n": req.quorum_n}


@app.post("/exceptions/approve")
def exception_approve(ap: ExceptionApprove):
    conn = db()
    row = conn.execute(
        "SELECT status, quorum_n, approvals_json FROM exc_cases WHERE case_id=?",
        (ap.case_id,),
    ).fetchone()
    if not row:
        conn.close()
        raise HTTPException(404, "Case not found.")
    status, quorum_n, approvals_json = row
    if status != "open":
        conn.close()
        raise HTTPException(400, f"Case is not open (status={status}).")

    # peer must exist
    p = conn.execute("SELECT 1 FROM peers WHERE peer_id=?", (ap.approver_peer_id,)).fetchone()
    if not p:
        conn.close()
        raise HTTPException(400, "Unknown approver_peer_id.")

    approvals = set(json.loads(approvals_json))
    approvals.add(ap.approver_peer_id)

    new_status = "approved" if len(approvals) >= quorum_n else "open"

    conn.execute(
        "UPDATE exc_cases SET approvals_json=?, status=? WHERE case_id=?",
        (json.dumps(sorted(approvals), separators=(",", ":")), new_status, ap.case_id),
    )
    conn.commit()
    conn.close()
    return {"ok": True, "case_id": ap.case_id, "status": new_status, "approvals": sorted(approvals), "quorum_n": quorum_n}


@app.get("/exceptions/{case_id}")
def exception_status(case_id: str):
    conn = db()
    row = conn.execute(
        "SELECT case_id, requested_by, target_event_hash, reason, created_at, status, quorum_n, approvals_json FROM exc_cases WHERE case_id=?",
        (case_id,),
    ).fetchone()
    conn.close()
    if not row:
        raise HTTPException(404, "Case not found.")
    return {
        "case_id": row[0],
        "requested_by": row[1],
        "target_event_hash": row[2],
        "reason": row[3],
        "created_at": row[4],
        "status": row[5],
        "quorum_n": row[6],
        "approvals": json.loads(row[7]),
    }


@app.post("/reveal")
def reveal(req: RevealRequest):
    """
    Formal exception trigger:
    - only reveals plaintext if case is approved
    - only reveals if event has encrypted content stored
    - does not reveal anything for external references (content_ref only)
    """
    conn = db()

    # requester must exist
    p = conn.execute("SELECT 1 FROM peers WHERE peer_id=?", (req.requester_peer_id,)).fetchone()
    if not p:
        conn.close()
        raise HTTPException(400, "Unknown requester_peer_id.")

    case = conn.execute(
        "SELECT status, target_event_hash FROM exc_cases WHERE case_id=?",
        (req.case_id,),
    ).fetchone()
    if not case:
        conn.close()
        raise HTTPException(404, "Case not found.")
    status, target_event_hash = case
    if status != "approved":
        conn.close()
        raise HTTPException(403, f"Case not approved (status={status}).")

    ev = conn.execute(
        "SELECT encrypted_blob_b64, content_ref, content_hash FROM events WHERE event_hash=?",
        (target_event_hash,),
    ).fetchone()
    conn.close()
    if not ev:
        raise HTTPException(404, "Target event not found.")
    enc_b64, content_ref, content_hash = ev

    if not enc_b64:
        # If content wasn't stored, we can only return references + hash (process audit)
        return {
            "ok": True,
            "case_id": req.case_id,
            "target_event_hash": target_event_hash,
            "reveal": None,
            "note": "No encrypted content stored; only content_ref + content_hash available.",
            "content_ref": content_ref,
            "content_hash": content_hash,
        }

    try:
        plaintext = FERNET.decrypt(b64d(enc_b64)).decode("utf-8")
    except InvalidToken:
        raise HTTPException(500, "Encrypted blob could not be decrypted (key mismatch or corruption).")

    return {
        "ok": True,
        "case_id": req.case_id,
        "target_event_hash": target_event_hash,
        "reveal": plaintext,
        "content_hash": content_hash,
        "content_ref": content_ref,
    }
