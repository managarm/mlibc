
asm ( ".section .text\n"
	".global _start\n"
		"_start:\n"
		"\tcall __mlibc_entry" );

