.section ".opd", "aw"
.align 8
.global _start
_start:
    .quad _realstart
    .quad .TOC.
    .quad 0

.text
_realstart:
    // todo(localcc): implement
    trap
    nop
    trap
.section .note.GNU-stack,"",%progbits

