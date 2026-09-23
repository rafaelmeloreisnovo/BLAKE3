/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * ARMv7 privileged HWIF leaf.
 * Compile/select only with RMR_ARMV7_ASSUME_PRIVILEGED.
 * These CP15 accesses must never be reached from ordinary user mode.
 */
.syntax unified
.text
.align 2

.global rmr_hwif_armv7_priv_time_raw
.type rmr_hwif_armv7_priv_time_raw, %function
rmr_hwif_armv7_priv_time_raw:
    /*
     * PMCCNTR: implementation-dependent 32-bit cycle counter.
     * Returned as uint64_t with zero high word. Wrap semantics are explicit.
     */
    mrc p15, 0, r0, c9, c13, 0
    mov r1, #0
    bx lr
.size rmr_hwif_armv7_priv_time_raw, .-rmr_hwif_armv7_priv_time_raw

.global rmr_hwif_armv7_priv_cpu_id_raw
.type rmr_hwif_armv7_priv_cpu_id_raw, %function
rmr_hwif_armv7_priv_cpu_id_raw:
    /*
     * uint64_t layout:
     *   high32 = MIDR
     *   low32  = MPIDR
     * This is a capability fingerprint, not a PUF or unique serial.
     */
    mrc p15, 0, r1, c0, c0, 0
    mrc p15, 0, r0, c0, c0, 5
    bx lr
.size rmr_hwif_armv7_priv_cpu_id_raw, .-rmr_hwif_armv7_priv_cpu_id_raw

.global rmr_hwif_armv7_priv_backend_probe
.type rmr_hwif_armv7_priv_backend_probe, %function
rmr_hwif_armv7_priv_backend_probe:
    mov r0, #1
    mov r1, #0
    bx lr
.size rmr_hwif_armv7_priv_backend_probe, .-rmr_hwif_armv7_priv_backend_probe

.section .note.GNU-stack,"",%progbits
