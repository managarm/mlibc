.section .init
    ld.d $ra, $sp, 8
    addi.d $sp, $sp, 16

.section .fini
    ld.d $ra, $sp, 8
    addi.d $sp, $sp, 16

.section .note.GNU-stack,"",%progbits
