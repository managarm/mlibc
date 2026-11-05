#include <internal-config.h>

.section .text
.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

_start:
	suba.l %fp, %fp
	move.l %sp, %d0
#if MLIBC_ENTRY_INVERTED_ORDER
	move.l %d0, -(%sp)
	move.l #main, -(%sp)
#else
	move.l #main, -(%sp)
	move.l %d0, -(%sp)
#endif
	jsr __mlibc_entry

.section .note.GNU-stack,"",%progbits
