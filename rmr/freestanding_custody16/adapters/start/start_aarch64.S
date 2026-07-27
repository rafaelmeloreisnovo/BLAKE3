.text
.globl _start
.type _start,%function
_start:
    mov x0,sp
    bl rmr_fc16_termux_entry
    mov x8,#93
    svc #0
.size _start,.-_start
