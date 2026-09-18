.text
.align 4
.global rmr_fixed256_reduce
.type rmr_fixed256_reduce,%function
/* x0=out16, x1=in256. Fully unrolled: no variable tail path, no external call. */
rmr_fixed256_reduce:
    movi v0.16b, #0
    .rept 16
    ld1 {v1.16b}, [x1], #16
    eor v0.16b, v0.16b, v1.16b
    .endr
    st1 {v0.16b}, [x0]
    ret
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
    adrp x0, rmr_fixed256_probe_out
    add  x0, x0, :lo12:rmr_fixed256_probe_out
    adrp x1, rmr_fixed256_probe_in
    add  x1, x1, :lo12:rmr_fixed256_probe_in
    bl rmr_fixed256_reduce
1:  b 1b
.size _start, .-_start
