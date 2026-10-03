#include "mlibc-asm/helpers.h"

PROC_START(_start)
	/* Align the stack to 128 bits per ABI */
	mv s1, sp
	andi sp, sp, -16

	call relocateSelf

	mv a0, s1
	call interpreterMain

	mv s1, sp
	jr a0
PROC_END(_start)

GNU_STACK_NOTE()

