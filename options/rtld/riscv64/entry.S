#include "mlibc-asm/helpers.h"

PROC_START(_start)
	call relocateSelf

	mv a0, sp
	call interpreterMain

	jr a0
PROC_END(_start)

GNU_STACK_NOTE()

