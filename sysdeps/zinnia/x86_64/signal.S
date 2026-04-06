.global __mlibc_restorer

__mlibc_restorer:
    mov $128, %rax /* SYSCALL_SIGRETURN */
    syscall
    ud2

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
