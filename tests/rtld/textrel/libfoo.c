static int target(void) {
	return 42;
}

// Place a pointer in .text, which requires a relocation in a non-writable segment.
__asm__(
	".pushsection .text\n"
	".globl textrel_ptr\n"
	".type textrel_ptr, %object\n"
	".p2align 3\n"
	"textrel_ptr:\n"
	".dc.a target\n"
	".size textrel_ptr, . - textrel_ptr\n"
	".popsection\n"
);

// Keep target() alive, it is only referenced from the inline assembly above.
int (*keep_target)(void) = target;
