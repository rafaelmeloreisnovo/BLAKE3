.syntax unified
.text
.globl _start
.type _start,%function
_start:
    mov r0,sp
    bl rmr_fc16_termux_entry
    mov r7,#1
    svc #0
.size _start,.-_start
