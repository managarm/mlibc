#pragma once

#if !defined(__ASSEMBLER__)
#error "This file can only be used by assembly files."
#endif

#define PROC_START(name) \
	.global name; \
	.type name, @function; \
	.cfi_startproc; \
	name:

#define PROC_END(name) \
	.cfi_endproc; \
	.size name, . - name
