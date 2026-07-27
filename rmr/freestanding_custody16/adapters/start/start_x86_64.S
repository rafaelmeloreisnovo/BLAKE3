.text
.globl _start
.type _start,@function
_start:
    mov %rsp,%rdi
    andq $-16,%rsp
    call rmr_fc16_termux_entry
    mov %eax,%edi
    mov $60,%eax
    syscall
.size _start,.-_start
