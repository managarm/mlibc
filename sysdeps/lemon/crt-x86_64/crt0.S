.section .text

.global _start
_start:
    mov $main, %rdi
    call __mlibc_entry

.size _start, . - _start
