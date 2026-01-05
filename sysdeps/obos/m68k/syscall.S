.global syscall

syscall:
    move.l %fp, %sp@-
    move.l %sp, %fp
    move.l %d2, %sp@-
    move.l %d3, %sp@-
    move.l %d4, %sp@-
    move.l %d5, %sp@-

    move.l (+8,%fp), %d5
    move.l (+12,%fp), %d0
    move.l (+16,%fp), %d1
    move.l (+20,%fp), %d2
    move.l (+24,%fp), %d3
    move.l (+28,%fp), %d4
    trap #0

    move.l %sp@+, %d5
    move.l %sp@+, %d4
    move.l %sp@+, %d3
    move.l %sp@+, %d2
    move.l %sp@+, %fp
    rts

.hidden syscall

#include <mlibc-asm/helpers.h>
GNU_STACK_NOTE()
