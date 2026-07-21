# RMR stability and visual-difference kernel

## Scope

This module consolidates mechanisms that were previously separated:

1. **Event-state stability** — portable C equivalent of the existing ARMv7 four-word state step.
2. **TRIAD trace association** — the GBS3 metric `DeltaP = P(stable_any | peak gate) - P(stable_any | non-peak gate)`.
3. **Visual descriptor difference** — Otsu threshold, foreground ratio, eight-bin angular distribution, corrected chi-square, directional concentration, CRC32C custody fingerprint and a deterministic difference score.

The layers remain distinct:

```text
L0 bytes/events   -> state score + CRC32C
L1 image geometry -> Otsu + angular histogram + descriptor difference
L2 trace evidence -> DeltaP association over labeled stable/peak events
```

`DeltaP` is an association statistic. It is not semantic accuracy, causal proof, or a universal attractor.

## Corrections captured by the audit

- Missing or incomplete traces use status flags, never an unexplained numeric zero.
- Angular chi-square uses the exact expected value `n/8` through `sum((8*obs-n)^2)/(8*n)`. It therefore works when `n < 8`; the previous integer `e=n/8` path falsely returned zero.
- Descriptor identity serializes complete 32-bit fields in canonical little-endian order before CRC32C. It does not truncate every value to one byte.
- The visual core is a deterministic descriptor/custody layer. It does not claim object recognition; a vision encoder or detector may provide angles/embeddings above it.

## Build

```sh
cc -O2 -std=c11 -Wall -Wextra -Werror -pedantic \
  -Irmr/include rmr/src/rmr_stability.c rmr/tests/rmr_stability_selftest.c \
  -o rmr_stability_selftest
./rmr_stability_selftest

cc -O2 -std=c11 -Wall -Wextra -Werror -pedantic \
  -Irmr/include rmr/src/rmr_stability.c rmr/tools/rmr_stability_trace_cli.c \
  -o rmr_stability_trace
./rmr_stability_trace out/triad_trace.csv
```

## Repository boundary

- **BLAKE3** hashes reports and artifacts as the public cryptographic integrity primitive.
- **RMR** owns operation snapshots, trace counts, visual descriptors, difference scores and custody metadata.
- **Vectra** consumes the portable path on every ABI and may dispatch to the pre-existing ARMv7 assembly only after equivalence testing.
- **LlamaRafaelia/CTI** remains the producer of `triad_trace.csv`; GBS3 can consume the same canonical result instead of keeping a private duplicate formula.

## Three-level growth

- **Bottom:** raw bytes/pixels, Otsu, event state, CRC32C.
- **Middle:** angular distribution, concentration and deterministic descriptor difference.
- **Top:** trace-level DeltaP and later labeled prototype/embedding evaluation. The top layer stays `TOKEN_VAZIO` until labels or a model output are actually supplied.
