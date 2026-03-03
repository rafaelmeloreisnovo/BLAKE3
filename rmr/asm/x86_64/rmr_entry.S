/*
 * Copyright (c) 2025 Rafael
 * License: RMR Module License (see rmr/LICENSE_RMR)
 */

.text
.global _start
_start:
    call main
    mov $60, %rax
    xor %rdi, %rdi
    syscall
