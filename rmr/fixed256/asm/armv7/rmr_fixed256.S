.syntax unified
.fpu neon
.text
.align 2
.global rmr_fixed256_reduce
.type rmr_fixed256_reduce,%function
/* r0=out16, r1=in256. Fully unrolled: no variable tail path, no external call. */
rmr_fixed256_reduce:
    veor q0, q0, q0
    .rept 16
    vld1.8 {q1}, [r1]!
    veor q0, q0, q1
    .endr
    vst1.8 {q0}, [r0]
    bx lr
.size rmr_fixed256_reduce, .-rmr_fixed256_reduce

.bss
.balign 16
rmr_fixed256_probe_in:
    .space 256
rmr_fixed256_probe_out:
    .space 16

.text
.global _start
.type _start,%function
_start:
    ldr r0, =rmr_fixed256_probe_out
    ldr r1, =rmr_fixed256_probe_in
    bl rmr_fixed256_reduce
1:  b 1b
.size _start, .-_start
