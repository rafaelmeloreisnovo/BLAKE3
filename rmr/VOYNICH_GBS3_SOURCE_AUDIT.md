# Source audit: GBS3 stability and Voynich visual kernels

## Material read

The consolidation was based on the complete supplied sources, not only on the two small `cognitio` examples:

- `gbs3.c`
- `gbs3_color.c`
- `voy_core.c`
- `voynich_angular.c` and its freestanding ZIP package
- `voynich_toroidal.c`
- `voynich_downloader.c`
- `voynich_exacordex.c`
- `voynich_analysis.py`
- `voy.py`

## GBS3: actual stability index

Both GBS3 variants calculate the same statistic from `out/triad_trace.csv`:

```text
DeltaP = P(stable_any | peak) - P(stable_any | non-peak)
peak := gate_in_peaks or gate in {3,4,8}
```

`gbs3_color.c` adds ANSI rendering and HUD switches. It does not alter the DeltaP formula. The arena then uses thresholds `DeltaP > 0.05` and `DeltaP > 0.20` as behavior bias. That use is a policy choice, not part of the statistical definition.

The original GBS3 reader returned numeric zero when the trace or required columns were missing. The canonical RMR kernel replaces that ambiguity with explicit status flags.

## Voynich angular/core: retained mechanisms

The useful deterministic seed is:

- PGM/P5 grayscale input;
- Otsu threshold selection;
- local foreground detection;
- terminal-stroke angle estimation;
- eight directional bins;
- angular non-uniformity;
- CRC32C custody over produced evidence;
- freestanding/static-memory execution intent.

These mechanisms became the visual descriptor layer. Object labels and semantic interpretation remain above this layer.

## Defects corrected

### Small-sample chi-square

The prior implementation used `expected = n / 8` and returned zero when `n < 8`. Four observations concentrated in one direction were therefore reported as uniform. The corrected rational form is:

```text
chi2 = sum((8*observed - n)^2) / (8*n)
```

### Truncated custody fields

The prior CRC loop fed only the low byte of coordinates and angle. The canonical descriptor serializes complete 32-bit fields in little-endian order before CRC32C.

### Synthetic angle test

The supplied freestanding synthetic test detected four glyphs but resolved every angle as 45 degrees with zero confidence. This proves that detection and angle extraction still require a separate calibration fixture; the RMR descriptor accepts measured angles but does not pretend the existing detector is validated.

## Files kept outside the canonical inference core

### `voynich_toroidal.c`

The current program is a stochastic simulation. RGB values are stored in signed `char`, which can generate negative channels and invalid CMYK values. It is useful as a visualization experiment, not as evidence or a production classifier.

### `voynich_downloader.c`

This is ingestion/extraction tooling and currently simulates RGB instead of decoding image content. It remains outside the inference kernel.

### `voynich_exacordex.c`

This downloads and counts substrings from a web page. It does not establish Voynich visual structure and is not part of the stability index.

### `voynich_analysis.py`

It contains real OpenCV operations for grayscale entropy, sampled CMYK and pairwise image difference. However, the configured toroidal modes effectively use the same linear stride and mainly differ by initial skip. Its feature operations may feed future adapters; its concluding claims are not promoted to proof.

### `voy.py`

This is a dataset downloader. It belongs to acquisition, not inference.

## Canonical growth path

```text
BOTTOM
raw image / event bytes
  -> Otsu, foreground, CRC32C, four-word event state

MIDDLE
measured angles / external visual embedding
  -> directional histogram, concentration, descriptor difference/hash

TOP
TRIAD trace or labeled prototypes
  -> DeltaP association, held-out recognition metrics, provenance report
```

The top visual-recognition result remains `TOKEN_VAZIO` until real labeled images or embeddings are supplied and evaluated on held-out examples.
