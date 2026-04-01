.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
    ldr x0, [sp], #8
    ldr x1, [sp], #8
    ldr x2, [sp], #8
    bl __mlibc_enter_thread

.section .note.GNU-stack,"",%progbits
