.section .init
.globl _init
.type _init, @function
_init:
    push %rbp
    mov %rsp, %rbp

.section .fini
.globl _fini
.type _fini, @function
_fini:
    push %rbp
    mov %rsp, %rbp

.section .note.GNU-stack,"",%progbits