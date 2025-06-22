.abiversion 1
.section ".opd", "aw"
.align 8
.global __mlibc_do_asm_cp_syscall
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
    .quad .L.__mlibc_do_asm_cp_syscall
    .quad .TOC.
    .quad 0
.global __mlibc_syscall_begin
.global __mlibc_syscall_end

.text
.L.__mlibc_do_asm_cp_syscall:
    mr 0, 3
    mr 3, 4
    mr 4, 5
    mr 5, 6
    mr 6, 7
    mr 7, 8
    mr 8, 9

    lwz 9, -0x7060(13) // Tcb::cancelBits
__mlibc_syscall_begin:
    li 10, ((1 << 0) | (1 << 2))
    and 9, 9, 10
    cmpd 9, 10
    beq cancel
    sc

    // linux sets cr0.SO on ppc64 if a syscall failed
    // but the rest of the library expects a negated result code in case of a failure
    // adjusting the return value
    mfcr 4
    rlwinm 4, 4, 4, 31, 31
    neg 5, 4
    xor 3, 3, 5
    add 3, 3, 4
__mlibc_syscall_end:
    blr

cancel:
    b __mlibc_do_cancel
    nop
    trap

.section .note.GNU-stack,"",%progbits

