
.section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
    ld a0, 0(sp)
    ld a1, 8(sp)
    ld a2, 16(sp)
    addi sp, sp, 32
    call __mlibc_thread_trampoline

.section .note.GNU-stack,"",@progbits
