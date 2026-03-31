.section .init
    ld ra, 8(sp)
    addi sp, sp, 16

.section .fini
    ld ra, 8(sp)
    addi sp, sp, 16

.section .note.GNU-stack,"",%progbits
