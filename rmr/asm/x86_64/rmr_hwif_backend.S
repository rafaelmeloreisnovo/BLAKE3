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

rmr_hwif_x86_64_cpu_id_raw:
    push %rbx
    mov $1, %eax
    xor %ecx, %ecx
    cpuid
    shl $32, %rbx
    mov %eax, %eax
    or %rbx, %rax
    pop %rbx
    ret

rmr_hwif_x86_64_raw_write:
    mov $1, %rax
    syscall
    ret

rmr_hwif_x86_64_backend_probe:
    mov $1, %rax
    ret
