#include <zinnia/syscall.hpp>

.global __mlibc_restorer

__mlibc_restorer:
    li.w $a7, SYSCALL_SIGRETURN
    syscall 0
    break 0

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
