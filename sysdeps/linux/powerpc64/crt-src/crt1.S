.section ".opd", "aw"
.align 8
.global _start
_start:
    .quad _realstart
    .quad .TOC.
    .quad 0

.text
_realstart:
    mr 3, 1

    addis 4, 2, main@toc@ha
    addi 4, 4, main@toc@l

    bl __mlibc_entry
    nop

.section .note.GNU-stack,"",%progbits

