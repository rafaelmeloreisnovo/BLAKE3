# H01 — SHA-256

State: AUTHORIAL_COMPRESSION_CORE_SOURCE / claim_allowed=false

SHA-256 is a standard primitive. RMR authors this source expression and freestanding methodology; it does not claim invention of SHA-256.

Implemented here:
- initialization state;
- one 64-byte compression function;
- unrolled 64-round source;
- no libc/heap/provider;
- local KAT fixture for the padded `abc` block.

Not implemented here yet:
- arbitrary-length streaming;
- padding/finalization API;
- complete boundary matrix.

Therefore this is not yet a complete replacement for the existing SHA-256 API.
