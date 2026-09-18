.text
.p2align 4
.global rmr_fixed256_reduce
.type rmr_fixed256_reduce,@function
/* SysV: rdi=out16, rsi=in256. Fully unrolled, SSE2 baseline. */
rmr_fixed256_reduce:
    pxor %xmm0, %xmm0
    .rept 16
    movdqu (%rsi), %xmm1
    pxor %xmm1, %xmm0
    add $16, %rsi
    .endr
    movdqu %xmm0, (%rdi)
    ret
.size rmr_fixed256_reduce, .-rmr_fixed256_reduce

.bss
.p2align 4
rmr_fixed256_probe_in:
    .space 256
rmr_fixed256_probe_out:
    .space 16

.text
.global _start
.type _start,@function
_start:
    lea rmr_fixed256_probe_out(%rip), %rdi
    lea rmr_fixed256_probe_in(%rip), %rsi
    call rmr_fixed256_reduce
1:  jmp 1b
.size _start, .-_start
