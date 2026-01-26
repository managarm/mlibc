.intel_syntax noprefix

.global __mlibc_restorer

#define OBOSS_SigReturn 48

__mlibc_restorer:
    mov rdi, rsp
    mov rax, OBOSS_SigReturn
    syscall
    // uh oh
    ud2

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
