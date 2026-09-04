#include "mlibc-asm/helpers.h"

PROC_START(_start)
.cfi_undefined eip
	call relocateSelf

	mov %esp, %esi
	lea -12(%esp), %esp
	push %esi
	call interpreterMain
	lea 16(%esp), %esp
	
	jmp *%eax
PROC_END(_start)

GNU_STACK_NOTE()
