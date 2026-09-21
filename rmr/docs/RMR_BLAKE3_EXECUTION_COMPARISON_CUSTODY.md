<!--
Copyright (c) 2024–2026 Rafael Melo Reis
Licensed under LICENSE_RMR.
-->

# RMR BLAKE3 — execution, comparison and custody navigation

State: IMPLEMENTED_PENDING_OBSERVABLE_CI  
Claim allowed: false

## I. Execution

RMR does not rename or reimplement BLAKE3.

The adapter in rmr/core/hash_blake3.c consumes the public BLAKE3 C API from c/.
The RMR CMake surface links against BLAKE3::blake3 and exposes:

~~~text
pai hash --algo blake3 --file FILE
pai scan --hash blake3 --base DIR --out OUT
~~~

Meaning:

~~~text
BLAKE3 = cryptographic primitive
RMR = adapter + execution + metadata + custody
~~~

## II. Comparison

rmr/tools/orchestrate_blake3_compare.sh builds two independently addressed source trees:

~~~text
official BLAKE3 pinned SHA
  -> official c/CMakeLists.txt
  -> official libblake3

fork/RMR checkout
  -> rmr/CMakeLists.txt
  -> repository c/ libblake3
  -> RMR adapter
~~~

Both libraries are exercised by the same benchmark source. The abc known-answer test and digest equivalence are mandatory gates before a performance summary exists.

## III. Custody

Each comparison campaign preserves:

- official and fork commit SHAs;
- compiler and CMake identity;
- environment;
- raw CSV results;
- correctness state;
- summary;
- receipt;
- SHA-256 checksums;
- BLAKE3 checksums.

Boundary:

~~~text
SOURCE != BUILD != EXECUTION != EVIDENCE != CLAIM
~~~

## Gate table

| Gate | State after source implementation |
| --- | --- |
| RMR BLAKE3 adapter | IMPLEMENTED |
| RMR CMake | IMPLEMENTED |
| Correctness KAT | NOT_RUN until execution |
| Fork vs pinned official harness | IMPLEMENTED |
| Automatic receipts | IMPLEMENTED |
| GitHub CI reproduction | NOT_RUN until workflow |
| Independent third-party reproduction | TOKEN_VAZIO |

## Termux

~~~sh
pkg install git clang cmake ninja
git clone https://github.com/rafaelmeloreisnovo/BLAKE3.git
cd BLAKE3
./rmr/tools/build_pai.sh
./rmr/tools/orchestrate_blake3_compare.sh
~~~

The orchestrator uses the official repository only as a pinned comparison source. It does not copy official code into rmr/.

## Claim discipline

A local or CI speed difference may be reported only with its commit, compiler, host, run matrix and receipt.

No source-level implementation in this change establishes universal superiority, a fixed speedup percentage, or independent third-party reproduction.
