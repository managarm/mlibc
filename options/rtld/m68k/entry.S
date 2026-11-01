#include "mlibc-asm/helpers.h"

PROC_START(_start)
	jbsr relocateSelf@PLTPC

	move.l %sp, -(%sp)
	jbsr interpreterMain@PLTPC
	addq.l #4, %sp

	jmp (%a0)
PROC_END(_start)

GNU_STACK_NOTE()
