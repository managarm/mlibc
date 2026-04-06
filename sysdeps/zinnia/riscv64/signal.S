.global __mlibc_restorer

__mlibc_restorer:
    li a7, 128 /* SYSCALL_SIGRETURN */
    ecall
    unimp

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
