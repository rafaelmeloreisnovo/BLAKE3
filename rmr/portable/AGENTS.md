# AGENTS.md — rmr/portable/

Scope: RMR Portable V1 only.

- Never modify upstream BLAKE3 to satisfy an RMR portability request.
- Preserve upstream attribution and license.
- New files in this tree use LicenseRef-RMR-Individual-Research-1.0 unless an
  explicit path matrix says otherwise.
- Pure core: no heap, libc call, syscall, filesystem, network, clock, thread,
  dynamic loader, JNI, Android API, or hidden provider lookup.
- Buffers are caller-owned.
- Fixed-size kernels should avoid variable tails and dynamic dispatch.
- Compile C with -Wshadow -Werror.
- Do not call JVM/Android/toolchain bytes “project-authored”.
- No automatic legal accusation, fine, or enforcement action.
- Preserve source SHA, flags, artifact hash, provider boundary, and uncertainty.
