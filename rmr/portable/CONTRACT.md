# RMR Portable Binary Contract V1

## Invariants

```text
SOURCE != PROVIDER != ADAPTER != BINARY != EXECUTION != EVIDENCE != CLAIM
UPSTREAM_BLAKE3 != RMR_PORTABLE
TOKEN_VAZIO != 0
IMPLEMENTED_UNTESTED != PASS
```

## Profiles

### PURE_FIXED_CORE
Fixed 256-byte frame. The scalar implementation is explicitly unrolled.
No variable tail exists.

### PROVIDER_BACKED_BLAKE3
Consumes the repository upstream BLAKE3 C implementation without rewriting the
algorithm. Provider source remains under upstream licensing.

### FREESTANDING_LINK
A link can be promoted only if:

```text
PT_INTERP absent
DT_NEEDED absent
unexpected UND = 0
forbidden symbols = 0
source + flags + compiler recorded
KAT/selftest PASS
```

## No-abstraction profile

For this profile, “no abstraction” means direct C ABI, no vtable, no plugin
loader, no dynamic provider lookup, no hidden fallback, no heap-owned opaque
object, and fixed memory ownership contracts.

Source languages require identifiers. The binary profile instead minimizes the
public symbol surface and uses hidden visibility + section GC.

The fixed256 kernel is unrolled and has no variable-length loop or tail.
Variable-length BLAKE3 necessarily has algorithmic iteration, therefore
`ZERO_LOOP_BLAKE3_CLAIM=PROHIBITED`.
