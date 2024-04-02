.section .text
.global _start
_start:
    mov %rsp, %rdi
    lea main(%rip), %rsi
    call __mlibc_entry
.section .note.GNU-stack,"",%progbits

