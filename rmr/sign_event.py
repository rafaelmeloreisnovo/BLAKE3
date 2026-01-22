# sign_event.py
import base64, json, time, hashlib
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey

def sha256_hex(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()

def canonical_payload(d: dict) -> bytes:
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
    return json.dumps(canon, sort_keys=True, separators=(",", ":")).encode()

PRIV_B64 = "..."  # from gen_peer_keys.py
PUB_B64  = "..."

priv = Ed25519PrivateKey.from_private_bytes(base64.urlsafe_b64decode(PRIV_B64))
ts = int(time.time())

event = {
    "peer_id": "peerA",
    "pubkey_b64": PUB_B64,
    "event_type": "MESSAGE",
    "scope": "private",
    "prev_hash": None,
    "timestamp": ts,
    "content_hash": sha256_hex(b"hello"),
    "content_ref": "vault://conversations/part_01#offset=123..456",
    "meta": {"yin_yang":"yin","domain":["biofotônica"],"err":"sigma"},
}

sig = priv.sign(canonical_payload(event))
event["signature_b64"] = base64.urlsafe_b64encode(sig).decode()

print(json.dumps(event, indent=2))
