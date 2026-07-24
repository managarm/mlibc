.section .text
.global _start
_start:
    mov %rsp, %rdi
    and $0xFFFFFFFFFFFFFFF0, %rsp
    mov $main, %rsi
    call __mlibc_entry
.section .note.GNU-stack,"",%progbits
