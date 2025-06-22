.abiversion 1
.section ".opd", "aw"
.align 8
.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
    .quad .L.__mlibc_signal_restore
    .quad .TOC.
    .quad 0
.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
    .quad .L.__mlibc_signal_restore_rt
    .quad .TOC.
    .quad 0

.text
.L.__mlibc_signal_restore:
.L.__mlibc_signal_restore_rt:
    li 0, 172 // __NR_rt_sigreturn
    sc

.section .note.GNU-stack,"",%progbits