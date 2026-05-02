/*
 * Copyright (c) 2025 Rafael
 * License: RMR Module License (see rmr/LICENSE_RMR)
 */

.section .text
.global _start
_start:
    bl main
    mov x0, #0
    mov x8, #93
    svc #0
