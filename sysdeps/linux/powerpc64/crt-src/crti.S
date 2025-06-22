.section .init
.global _init
_init:
    // todo(localcc): implement
    trap

.section .fini
.global _fini
_fini:
    // todo(localcc): implement
    trap
.section .note.GNU-stack,"",%progbits

