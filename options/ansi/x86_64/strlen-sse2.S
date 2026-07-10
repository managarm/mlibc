#include <mlibc-asm/helpers.h>

# size_t __mlibc_strlen_sse2(const char *s);
PROC_START(__mlibc_strlen_sse2)
	pxor %xmm0, %xmm0
	# char *p = s;
	movq %rdi, %rcx

	movq %rdi, %rax
	andl $4095, %eax
	cmpl $4080, %eax
	ja LABEL(page_cross)

	movdqu (%rcx), %xmm1
	pcmpeqb %xmm0, %xmm1
	pmovmskb %xmm1, %eax
	testl %eax, %eax
	jnz LABEL(found_near_start)

	# Advance p to the next 16-byte boundary: p = (p | 15) + 1
	orq $15, %rcx
	incq %rcx
	jmp LABEL(align_to_64_bytes_loop)

LABEL(page_cross):
	# Strategy: we calculate a `shift`, i.e. how many low bytes of the 16-byte
	# chunk we ignore due to being OOB. Then, we do a comparison of a 16-byte chunk,
	# and shift out those unwanted bits.

	# shift = (s & 15);
	movq %rdi, %rcx
	andq $15, %rcx

	movq %rdi, %rdx
	andq $-16, %rdx
	movdqa (%rdx), %xmm1
	pcmpeqb %xmm0, %xmm1
	pmovmskb %xmm1, %eax

	# mask >>= shift
	shrl %cl, %eax
	testl %eax, %eax
	jnz LABEL(found_near_start)

	movq %rdx, %rcx
	addq $16, %rcx
	# fallthrough to 64 byte alignment logic

LABEL(align_to_64_bytes_loop):
	testl $63, %ecx
	jz LABEL(main_loop)

	# %rcx is always 16-byte aligned
	movdqa (%rcx), %xmm1
	pcmpeqb %xmm0, %xmm1
	pmovmskb %xmm1, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk)

	addq $16, %rcx
	jmp LABEL(align_to_64_bytes_loop)

LABEL(found_near_start):
	tzcntl %eax, %eax
	ret

.align 16
LABEL(main_loop):
	# load 4 sequential 16-byte chunks
	movdqa (%rcx), %xmm1
	movdqa 16(%rcx), %xmm2
	movdqa 32(%rcx), %xmm3
	movdqa 48(%rcx), %xmm4

	# combine them using unsigned byte minimum
	movdqa %xmm1, %xmm5
	pminub %xmm2, %xmm5
	movdqa %xmm3, %xmm6
	pminub %xmm4, %xmm6
	pminub %xmm6, %xmm5

	# Get mask of the pooled zeroes
	pcmpeqb %xmm0, %xmm5
	pmovmskb %xmm5, %eax
	testl %eax, %eax
	jnz LABEL(resolve_pooled_zero)

	addq $64, %rcx
	jmp LABEL(main_loop)

LABEL(resolve_pooled_zero):
	pcmpeqb %xmm0, %xmm1
	pmovmskb %xmm1, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk1)

	pcmpeqb %xmm0, %xmm2
	pmovmskb %xmm2, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk2)

	pcmpeqb %xmm0, %xmm3
	pmovmskb %xmm3, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk3)

	pcmpeqb %xmm0, %xmm4
	pmovmskb %xmm4, %eax
	tzcntl %eax, %eax
	addq $48, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk1):
	tzcntl %eax, %eax
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk2):
	tzcntl %eax, %eax
	addq $16, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk3):
	tzcntl %eax, %eax
	addq $32, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk):
	# return (p + trailing_zeroes) - s;
	# where p points to the starting address of the current 16-byte chunk
	tzcntl %eax, %eax
LABEL(found_relative_to_s):
	# return (p + trailing_zeroes) - s;
	# where p points to the starting address of the current 16-byte chunk
	addq %rcx, %rax
	subq %rdi, %rax
	ret

PROC_END(__mlibc_strlen_sse2)

GNU_STACK_NOTE()
