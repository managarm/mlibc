.global __mlibc_thread_entry

// TODO: __mlibc_thread_entry
__mlibc_thread_entry:
    bra.l __mlibc_thread_entry
    
#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
