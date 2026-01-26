/* ---------------------------------------------------------
 * RAFAELIA OMEGA - ARM64 (no-libc) :: SYS + REGS
 * --------------------------------------------------------- */
.section .text
.global _start
.global sys_write
.global sys_openat
.global sys_close
.global sys_fsync
.global get_cntvct
.global get_midr
.global get_mpidr

_start:
    bl main
    mov x0, #0
    mov x8, #93          // exit
    svc #0

// write(fd, buf, len)
sys_write:
    mov x8, #64
    svc #0
    ret

// openat(dfd, path, flags, mode)
sys_openat:
    mov x8, #56
    svc #0
    ret

sys_close:
    mov x8, #57
    svc #0
    ret

sys_fsync:
    mov x8, #82          // fsync(fd)
    svc #0
    ret

get_cntvct:
    mrs x0, cntvct_el0
    ret

get_midr:
    mrs x0, midr_el1
    ret

get_mpidr:
    mrs x0, mpidr_el1
    ret
