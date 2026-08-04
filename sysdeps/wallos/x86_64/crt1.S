.section .text
.global _start
.type _start, @function

_start:
    mov %rsp, %rdi        # Initial stack pointer
    lea main(%rip), %rsi  # Address of main (PIC-safe)
    call __mlibc_entry

    # Should never return.
1:
    hlt
    jmp 1b

.section .note.GNU-stack,"",%progbits