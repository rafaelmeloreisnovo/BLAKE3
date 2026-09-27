# Kernel Substrate V1

This is **not** a cryptographic primitive by itself.

It provides fixed-shape operations needed by future standard-conformant implementations:
- little-endian loads/stores;
- XOR blocks 16/32/64;
- 32/64-bit rotate;
- modular word addition;
- mask select;
- fixed-size difference scans.

Source-shape V1 rejects `if/for/while` statements in the kernel/header and rejects hosted/heap tokens.

## Important

The absence of source control statements does not prove constant-time behavior.

Compiler output, memory access, cache behavior and target ISA still require separate review.
