/*
 * Copyright (c) 2024–2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 */
.section .text
.global rmr_hwif_aarch64_time_raw
.global rmr_hwif_aarch64_cpu_id_raw
.global rmr_hwif_aarch64_midr_raw
.global rmr_hwif_aarch64_mpidr_raw
.global rmr_hwif_aarch64_raw_write
.global rmr_hwif_aarch64_backend_probe

rmr_hwif_aarch64_time_raw:
    mrs x0, cntvct_el0
    ret

/*
 * Historical AUTO-ID64 contract recovered from the tracked AArch64 core.o:
 *   bits 63..32 = MIDR_EL1[31..0]
 *   bits 31..0  = MPIDR_EL1[31..0]
 */
rmr_hwif_aarch64_cpu_id_raw:
    mrs x1, midr_el1
    mrs x0, mpidr_el1
    bfi x0, x1, #32, #32
    ret

rmr_hwif_aarch64_midr_raw:
    mrs x0, midr_el1
    ret

rmr_hwif_aarch64_mpidr_raw:
    mrs x0, mpidr_el1
    ret

rmr_hwif_aarch64_raw_write:
    mov x8, #64
    svc #0
    ret

rmr_hwif_aarch64_backend_probe:
    mov x0, #1
    ret
