.text
.global _start
_start:
    call main
    mov $60, %rax
    xor %rdi, %rdi
    syscall
