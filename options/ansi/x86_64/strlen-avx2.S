#include <mlibc-asm/helpers.h>

# size_t __mlibc_strlen_avx2(const char *s);
PROC_START(__mlibc_strlen_avx2)
	vpxor %xmm0, %xmm0, %xmm0
	# char *p = s;
	movq %rdi, %rcx

	movq %rdi, %rax
	andl $4095, %eax
	cmpl $4064, %eax
	ja LABEL(page_cross)

	vmovdqu (%rcx), %ymm1
	vpcmpeqb %ymm0, %ymm1, %ymm1
	vpmovmskb %ymm1, %eax
	testl %eax, %eax
	jnz LABEL(found_near_start)

	# Advance p to the next 32-byte boundary: p = (p | 31) + 1
	orq $31, %rcx
	incq %rcx
	jmp LABEL(align_to_128_bytes_loop)

LABEL(page_cross):
	# Strategy: we calculate a `shift`, i.e. how many low bytes of the 32-byte
	# chunk we ignore due to being OOB. Then, we do a comparison of a 32-byte chunk,
	# and shift out those unwanted bits.

	# shift = (s & 31);
	movq %rdi, %rdx
	andq $31, %rdx

	andq $-32, %rcx
	vmovdqa (%rcx), %ymm1
	vpcmpeqb %ymm0, %ymm1, %ymm1
	vpmovmskb %ymm1, %eax

	# mask >>= shift
	shrx %edx, %eax, %eax
	testl %eax, %eax
	jnz LABEL(found_near_start)

	addq $32, %rcx
	# fallthrough to 128 byte alignment logic

LABEL(align_to_128_bytes_loop):
	testl $127, %ecx
	jz LABEL(main_loop)

	# %rcx is always 32-byte aligned
	vmovdqa (%rcx), %ymm1
	vpcmpeqb %ymm0, %ymm1, %ymm1
	vpmovmskb %ymm1, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk)

	addq $32, %rcx
	jmp LABEL(align_to_128_bytes_loop)

LABEL(found_near_start):
	tzcntl %eax, %eax
	vzeroupper
	ret

.align 16
LABEL(main_loop):
	# load 4 sequential 32-byte chunks
	vmovdqa (%rcx), %ymm1
	vmovdqa 32(%rcx), %ymm2
	vmovdqa 64(%rcx), %ymm3
	vmovdqa 96(%rcx), %ymm4

	# combine them using unsigned byte minimum
	vpminub %ymm2, %ymm1, %ymm5
	vpminub %ymm4, %ymm3, %ymm6
	vpminub %ymm6, %ymm5, %ymm5

	# Get mask of the pooled zeroes
	vpcmpeqb %ymm0, %ymm5, %ymm5
	vpmovmskb %ymm5, %eax
	testl %eax, %eax
	jnz LABEL(resolve_pooled_zero)

	addq $128, %rcx
	jmp LABEL(main_loop)

LABEL(resolve_pooled_zero):
	vpcmpeqb %ymm0, %ymm1, %ymm1
	vpmovmskb %ymm1, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk1)

	vpcmpeqb %ymm0, %ymm2, %ymm2
	vpmovmskb %ymm2, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk2)

	vpcmpeqb %ymm0, %ymm3, %ymm3
	vpmovmskb %ymm3, %eax
	testl %eax, %eax
	jnz LABEL(found_in_chunk3)

	vpcmpeqb %ymm0, %ymm4, %ymm4
	vpmovmskb %ymm4, %eax
	tzcntl %eax, %eax
	addq $96, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk1):
	tzcntl %eax, %eax
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk2):
	tzcntl %eax, %eax
	addq $32, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk3):
	tzcntl %eax, %eax
	addq $64, %rcx
	jmp LABEL(found_relative_to_s)

LABEL(found_in_chunk):
	# return (p + trailing_zeroes) - s;
	# where p points to the starting address of the current 32-byte chunk
	tzcntl %eax, %eax
	# fallthrough
LABEL(found_relative_to_s):
	# return (p + trailing_zeroes) - s;
	# where p points to the starting address of the current 32-byte chunk
	addq %rcx, %rax
	subq %rdi, %rax
	vzeroupper
	ret

PROC_END(__mlibc_strlen_avx2)

GNU_STACK_NOTE()
