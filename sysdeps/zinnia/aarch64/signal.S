.global __mlibc_restorer

__mlibc_restorer:
    mov x8, #128 /* SYSCALL_SIGRETURN */
    svc 0
    udf #0

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
