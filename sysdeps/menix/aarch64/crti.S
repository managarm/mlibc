.section .init
.global _init
_init:
    stp x29, x30, [sp, -16]!
    mov x29, sp

.section .fini
.global _fini
_fini:
    stp x29, x30, [sp, -16]!
    mov x29, sp

.section .note.GNU-stack,"",%progbits
