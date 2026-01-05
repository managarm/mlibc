.global __mlibc_restorer

#define OBOSS_SigReturn 48

// TODO: __mlibc_restorer
__mlibc_restorer:
    bra.l __mlibc_restorer

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
