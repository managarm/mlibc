#include <zinnia/syscall.hpp>

.global __mlibc_restorer

__mlibc_restorer:
    mov $SYSCALL_SIGRETURN, %rax
    syscall
    ud2

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
