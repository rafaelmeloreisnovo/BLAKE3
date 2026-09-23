<!--
Copyright (c) 2024-2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR IOPS Protocol V1

State: HARNESS_IMPLEMENTED_PHYSICAL_DEVICE_PENDING  
Claim allowed: false

## Identity

```text
logical IO != physical IOPS
CI smoke IOPS != device benchmark
page-cache result != raw-storage result
queue_depth=1 != asynchronous queueing
```

The host harness is `rmr/benchmark_framework/core/iops_bench.c`.

Every receipt records at least:

- architecture;
- read or write mode;
- sequential or deterministic-random pattern;
- block size;
- file size;
- requested/completed operation count;
- queue depth;
- sync policy;
- cache policy;
- elapsed monotonic time;
- IOPS;
- MiB/s;
- direct-I/O state.

## V1 constraints

Queue depth is deliberately limited to 1. Any value greater than 1 exits with
`TOKEN_VAZIO_V1`; the harness does not simulate asynchronous concurrency.

The cache policies are:

- `os-default`: no cache-state claim;
- `warm`: pre-read the file before the timed interval.

There is no `cold` claim because reliably dropping filesystem/device caches
requires platform-specific authority and evidence.

V1 does not use O_DIRECT, so a result may include page-cache effects. The JSON
receipt records `direct_io=false`.

## Example

```sh
rmr-iops-bench \
  --file /tmp/rmr-iops.bin \
  --mode read \
  --pattern random \
  --block-size 4096 \
  --file-size 16777216 \
  --ops 4096 \
  --sync none \
  --cache warm \
  --queue-depth 1
```

A CI execution validates the instrument and JSON path only. Performance claims
require a device-bound receipt with storage model, filesystem, kernel, thermal
state and repeated runs.
