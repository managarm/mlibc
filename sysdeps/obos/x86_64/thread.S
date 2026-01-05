.intel_syntax noprefix

.global __mlibc_thread_entry
.extern __mlibc_thread_trampoline

__mlibc_thread_entry:
    pop rsi // TCB
    pop rdi // Entry
    pop rdx // Userdata
    jmp __mlibc_thread_trampoline

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
