#include "mlibc-asm/helpers.h"

PROC_START(_start)
	bl relocateSelf

	mov x0, sp
	bl interpreterMain

	br x0
PROC_END(_start)

GNU_STACK_NOTE()

