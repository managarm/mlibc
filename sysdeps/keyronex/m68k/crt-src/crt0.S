.section .text
.global _start
_start:
    move.l #main, -(%sp)
    jsr __mlibc_entry

.section .note.GNU-stack,"",%progbits
