/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
.text
.global rmr_hwif_x86_64_time_raw
.global rmr_hwif_x86_64_cpu_id_raw
.global rmr_hwif_x86_64_raw_write
.global rmr_hwif_x86_64_backend_probe

rmr_hwif_x86_64_time_raw:
    rdtsc
    shl $32, %rdx
    or %rdx, %rax
    ret

/*
 * rmr_hwif_x86_64_cpu_id_raw contract:
 * - Returns a deterministic 64-bit CPU feature fingerprint in RAX.
 * - Collects CPUID leaf 0x00000001 (EAX/EBX/ECX/EDX) and
 *   leaf 0x00000007, subleaf 0 (EAX/EBX/ECX/EDX).
 * - Folds register pairs into 64-bit lanes and applies stable xor/rot/shift
 *   mixing so callers get a repeatable capability key (not a unique serial).
 * - Preserves ABI by saving/restoring RBX (callee-saved in SysV x86-64).
 */
rmr_hwif_x86_64_cpu_id_raw:
    push %rbx

    /* Leaf 1, subleaf 0: capture baseline feature registers. */
    mov $1, %eax
    xor %ecx, %ecx
    cpuid

    mov %eax, %r8d
    mov %ebx, %r9d
    mov %ecx, %r10d
    mov %edx, %r11d

    /* Leaf 7, subleaf 0: capture extended feature registers. */
    mov $7, %eax
    xor %ecx, %ecx
    cpuid

    /* Lane0 = (leaf1.EBX << 32) | leaf1.EAX. */
    mov %r9, %rax
    shl $32, %rax
    or %r8, %rax

    /* Lane1 = (leaf1.EDX << 32) | leaf1.ECX. */
    mov %r11, %rdx
    shl $32, %rdx
    or %r10, %rdx
    rol $13, %rdx
    xor %rdx, %rax

    /* Lane2 = (leaf7.EBX << 32) | leaf7.EAX. */
    mov %eax, %r8d
    mov %rbx, %rdx
    shl $32, %rdx
    or %r8, %rdx
    rol $29, %rdx
    xor %rdx, %rax

    /* Lane3 = (leaf7.EDX << 32) | leaf7.ECX. */
    mov %edx, %r11d
    mov %ecx, %r10d
    mov %r11, %rdx
    shl $32, %rdx
    or %r10, %rdx
    rol $17, %rdx
    xor %rdx, %rax

    /* Final avalanche to reduce linearity. */
    mov %rax, %rdx
    shr $33, %rdx
    xor %rdx, %rax
    mov %rax, %rdx
    shl $11, %rdx
    xor %rdx, %rax

    pop %rbx
    ret

rmr_hwif_x86_64_raw_write:
    mov $1, %rax
    syscall
    ret

rmr_hwif_x86_64_backend_probe:
    mov $1, %rax
    ret
