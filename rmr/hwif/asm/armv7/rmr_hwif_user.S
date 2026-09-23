/*
 * Copyright (c) 2024-2026 Rafael Melo Reis
 * Licensed under LICENSE_RMR.
 *
 * ARMv7 Linux/Android user-mode HWIF leaf.
 * No privileged CP15 register is read here.
 */
.syntax unified
.text
.align 2

.global rmr_hwif_armv7_user_raw_write
.type rmr_hwif_armv7_user_raw_write, %function
rmr_hwif_armv7_user_raw_write:
    /*
     * AAPCS32:
     *   r0 = fd
     *   r1 = buf
     *   r2:r3 = uint64_t len, low word in r2
     * Linux ARM EABI write(2): r7=4, args r0/r1/r2.
     * Return int64_t: sign-extend syscall long result into r1:r0.
     */
    push {r7, lr}
    mov r7, #4
    svc #0
    asr r1, r0, #31
    pop {r7, pc}
.size rmr_hwif_armv7_user_raw_write, .-rmr_hwif_armv7_user_raw_write

.global rmr_hwif_armv7_user_backend_probe
.type rmr_hwif_armv7_user_backend_probe, %function
rmr_hwif_armv7_user_backend_probe:
    mov r0, #1
    mov r1, #0
    bx lr
.size rmr_hwif_armv7_user_backend_probe, .-rmr_hwif_armv7_user_backend_probe
