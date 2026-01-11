.global __mlibc_restorer

#define OBOSS_SigReturn 48

__mlibc_restorer:
    bra.l __mlibc_restorer

.section .note.GNU-stack,"",%progbits
