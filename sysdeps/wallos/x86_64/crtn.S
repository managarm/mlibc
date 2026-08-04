.section .init
    pop %rbp
    ret

.section .fini
    pop %rbp
    ret

.section .note.GNU-stack,"",%progbits