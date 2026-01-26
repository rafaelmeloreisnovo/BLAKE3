/* ---------------------------------------------------------
 * RAFAELIA_SYNC_OMEGA - ASM ARM64 (no-libc)
 * syscalls + registradores de tempo + auto-id
 * --------------------------------------------------------- */
.section .text
.global _start
.global sys_write
.global sys_openat
.global sys_read
.global sys_close
.global rd_cntfrq
.global rd_cntvct
.global rd_midr

_start:
    bl main
    mov x0, #0
    mov x8, #93         // exit (aarch64)
    svc #0

// write(fd=x0, buf=x1, len=x2)
sys_write:
    mov x8, #64
    svc #0
    ret

// openat(dfd=x0, path=x1, flags=x2, mode=x3)
sys_openat:
    mov x8, #56
    svc #0
    ret

// read(fd=x0, buf=x1, len=x2)
sys_read:
    mov x8, #63
    svc #0
    ret

// close(fd=x0)
sys_close:
    mov x8, #57
    svc #0
    ret

rd_cntfrq:
    mrs x0, cntfrq_el0
    ret

rd_cntvct:
    mrs x0, cntvct_el0
    ret

rd_midr:
    mrs x0, midr_el1
    ret
