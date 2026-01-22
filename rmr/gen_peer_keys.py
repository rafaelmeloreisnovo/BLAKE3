# gen_peer_keys.py
from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
import base64

priv = Ed25519PrivateKey.generate()
pub = priv.public_key()

priv_bytes = priv.private_bytes(
    encoding=__import__("cryptography.hazmat.primitives.serialization").hazmat.primitives.serialization.Encoding.Raw,
    format=__import__("cryptography.hazmat.primitives.serialization").hazmat.primitives.serialization.PrivateFormat.Raw,
    encryption_algorithm=__import__("cryptography.hazmat.primitives.serialization").hazmat.primitives.serialization.NoEncryption(),
)
pub_bytes = pub.public_bytes(
    encoding=__import__("cryptography.hazmat.primitives.serialization").hazmat.primitives.serialization.Encoding.Raw,
    format=__import__("cryptography.hazmat.primitives.serialization").hazmat.primitives.serialization.PublicFormat.Raw,
)

print("PRIV_B64 =", base64.urlsafe_b64encode(priv_bytes).decode())
print("PUB_B64  =", base64.urlsafe_b64encode(pub_bytes).decode())
